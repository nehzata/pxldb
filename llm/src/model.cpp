#include "model.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>

#include "common/common.h"
#include "llama.h"
#include "utils.h"

model::~model() {
    if (current_task != NULL) {
        delete current_task;
    }
    llama_batch_free(batch);
    llama_free(ctx);
    llama_free_model(m);
    llama_backend_free();
}

model::model(std::string mpath, int n_ctx, int n_batch, int n_threads)
    : mpath(mpath), n_ctx(n_ctx), n_batch(n_batch), n_threads(n_threads), g_req_id(1), current_schema_id(0) {}

int model::init() {
    spdlog::info("starting model={}, ctx={}", mpath, n_ctx);
    llama_backend_init(false);

    llama_model_params mparams = llama_model_default_params();
    mparams.n_gpu_layers = 9999;
    m = llama_load_model_from_file(mpath.c_str(), mparams);

    this->n_ctx = std::min(n_ctx == 0 ? llama_n_ctx_train(m) : n_ctx, llama_n_ctx_train(m));

    llama_context_params cparams = llama_context_default_params();
    cparams.n_batch = n_batch;
    cparams.n_threads = n_threads;
    cparams.n_ctx = n_ctx;

    ctx = llama_new_context_with_model(m, cparams);
    if (ctx == NULL) {
        spdlog::error("failed to create llama_context");
        return 1;
    }

    batch = llama_batch_init(n_ctx, 0, 1);

    current_task = NULL;

    return 0;
}

void model::set_schema(uint schema_id, std::string &schema) {
    if (current_schema_id == schema_id) {
        return;
    }

    // tokenize the prompt outside the lock scope
    std::vector<llama_token> tokens = tokenize(schema);

    std::unique_lock<std::mutex> lock(mtx_queue);

    current_schema_id = schema_id;

    // cancel all tasks in the queue or currently running
    if (current_task != NULL && current_task->state <= STATE_DECODE) {
        current_task->state = STATE_CANCEL;
    }
    for (auto t : queue) {
        if (t->state <= STATE_DECODE) {
            t->state = STATE_CANCEL;
        }
    }

    int task_id = g_req_id++;

    task *t = new task(task_id, TASK_SET_SCHEMA, schema_id, tokens);
    queue.push_back(t);

    lock.unlock();
    c_queue.notify_one();
}

uint model::prime(uint schema_id, std::string &prompt) {
    // tokenize the prompt outside the lock scope
    std::vector<llama_token> tokens = tokenize(prompt);

    std::unique_lock<std::mutex> lock(mtx_queue);

    if (current_schema_id != schema_id) {
        return 0;
    }

    // cancel all tasks in the queue or currently running
    // except schema tasks
    if (current_task != NULL && current_task->type != TASK_SET_SCHEMA && current_task->state <= STATE_DECODE) {
        spdlog::info("prime {}: cancelling", current_task->id);
        current_task->state = STATE_CANCEL;
    }
    for (auto t : queue) {
        if (t->type != TASK_SET_SCHEMA && t->state <= STATE_DECODE) {
            spdlog::info("prime {}: cancelling", t->id);
            t->state = STATE_CANCEL;
        }
    }

    uint task_id = g_req_id++;

    task *t = new task(task_id, TASK_PRIME, schema_id, tokens);
    queue.push_back(t);

    lock.unlock();
    c_queue.notify_one();

    return task_id;
}

uint model::autocomplete(uint schema_id, std::string &prompt) {
    // tokenize the prompt outside the lock scope
    std::vector<llama_token> tokens = tokenize(prompt);

    std::unique_lock<std::mutex> lock(mtx_queue);

    if (current_schema_id != schema_id) {
        return 0;
    }

    // cancel all tasks in the queue or currently running
    // except schema tasks
    if (current_task != NULL && current_task->type != TASK_SET_SCHEMA && current_task->state <= STATE_DECODE) {
        spdlog::info("autocomplete {}: cancelling", current_task->id);
        current_task->state = STATE_CANCEL;
    }
    for (auto t : queue) {
        if (t->type != TASK_SET_SCHEMA && t->state <= STATE_DECODE) {
            spdlog::info("autocomplete {}: cancelling", t->id);
            t->state = STATE_CANCEL;
        }
    }

    uint task_id = g_req_id++;

    task *t = new task(task_id, TASK_AUTOCOMPLETE, schema_id, tokens);
    queue.push_back(t);

    lock.unlock();
    c_queue.notify_one();

    return task_id;
}

bool model::next_result(uint task_id, task_result &r) {
    spdlog::debug("next_result {}", task_id);
    for (int i = 0; i < 3; i++) {
        std::unique_lock<std::mutex> lock(mtx_results);
        if (!c_results.wait_for(lock, std::chrono::seconds(10),
                                [&] { return results.find(task_id) != results.end(); })) {
            spdlog::error("next_result {}: timed out", task_id);
            continue;
        }

        std::map<uint, task_result>::iterator it = results.find(task_id);
        r = it->second;
        results.erase(it);
        return true;
    }

    return false;
}

void model::run() {
    // wait until current task is non-null or queue is not empty
    if (current_task == NULL && queue.empty()) {
        spdlog::info("run: queue is empty");

        std::unique_lock<std::mutex> qlock(mtx_queue);
        if (!c_queue.wait_for(qlock, std::chrono::seconds(10), [&] { return !queue.empty(); })) {
            return;
        }
    }

    // obtain a lock across the queue
    std::unique_lock<std::mutex> qlock(mtx_queue);

    spdlog::info("run: queue size {}", queue.size());

    // if there's no task currently in progress
    // pop the next task in the queue
    if (current_task == NULL) {
        std::vector<task *>::iterator it = queue.begin();

        current_task = *it;

        // pop task from queue
        queue.erase(it);
    }

    // if the task is done, trigger the result queue with appropriate value
    // and move the task into STATE_STOP so it gets cleaned up
    if (current_task->state == STATE_DONE) {
        spdlog::info("run {}: done", current_task->id);

        // respond back to user
        if (current_task->type != TASK_SET_SCHEMA) {
            task_result res;
            res.stop = true;
            std::unique_lock<std::mutex> rlock(mtx_results);
            results.insert(std::pair<uint, task_result>(current_task->id, res));
            rlock.unlock();
            c_results.notify_all();
        }

        current_task->state = STATE_STOP;
    }

    // if the task is errored, trigger the result queue with appropriate value
    // and move the task into STATE_STOP so it gets cleaned up
    if (current_task->state == STATE_ERROR) {
        spdlog::info("run {}: error", current_task->id);

        // respond back to user
        if (current_task->type != TASK_SET_SCHEMA) {
            task_result res;
            res.error = true;

            std::unique_lock<std::mutex> rlock(mtx_results);
            results.insert(std::pair<uint, task_result>(current_task->id, res));
            rlock.unlock();
            c_results.notify_all();
        }

        current_task->state = STATE_STOP;
    }

    // if the task is cancelled, trigger the result queue with appropriate value
    // and move the task into STATE_STOP so it gets cleaned up
    if (current_task->state == STATE_CANCEL) {
        spdlog::info("run {}: cancelled", current_task->id);

        // respond back to user
        if (current_task->type != TASK_SET_SCHEMA) {
            task_result res;
            res.cancel = true;

            std::unique_lock<std::mutex> rlock(mtx_results);
            results.insert(std::pair<uint, task_result>(current_task->id, res));
            rlock.unlock();
            c_results.notify_all();
        }

        current_task->state = STATE_STOP;
    }

    // stopped tasks need to be deleted
    if (current_task->state == STATE_STOP) {
        spdlog::info("run {}: stopped", current_task->id);
        // if current task needs to be stopped
        // delete it and return
        delete current_task;
        current_task = NULL;
        return;
    }

    // unlock the queue so server requests can add to it
    // we won't be modifying current_task pointer & queue from this point
    // forward.
    qlock.unlock();

    // print_tokens("schema tokens", cache_schema_tokens);
    // print_tokens("prompt tokens", cache_prompt_tokens);

    // if we're just starting processing this task
    // then we need to load the prompt into the model
    // we try to retain as much of the previous cache values
    // as possible.
    if (current_task->state == STATE_LOADING) {
        // reset the batch
        llama_batch_clear(batch);

        // try to reuse the cache as much as possible
        switch (current_task->type) {
            case TASK_SET_SCHEMA: {
                size_t n_common = common_part(cache_schema_tokens, current_task->prompt_tokens);
                llama_kv_cache_seq_rm(ctx, 0, n_common, -1);
                current_task->n_pos_cache = n_common;

                // remove the uncommon tokens from cache_schema_tokens
                cache_schema_tokens.resize(n_common);

                // empty out the cache_prompt_tokens
                cache_prompt_tokens.clear();
                cache_sampling_states.clear();

                break;
            }
            default: {
                size_t n_common = common_part(cache_prompt_tokens, current_task->prompt_tokens);

                if (current_task->type == TASK_AUTOCOMPLETE && n_common > 0 &&
                    n_common == current_task->prompt_tokens.size()) {
                    n_common--;
                }

                size_t n_pos = cache_schema_tokens.size() + n_common;
                llama_kv_cache_seq_rm(ctx, 0, n_pos, -1);
                current_task->n_pos_cache = n_pos;

                // remove the uncommon tokens from cache_prompt_tokens
                cache_prompt_tokens.resize(n_common);
                cache_sampling_states.resize(n_common);

                break;
            }
        }

        // push the non-cached/uncommon remainder of the prompt into the model
        switch (current_task->type) {
            case TASK_SET_SCHEMA: {
                for (size_t i = current_task->n_pos_cache; i < current_task->prompt_tokens.size(); i++) {
                    llama_batch_add(batch, current_task->prompt_tokens[i], i, {0}, false);
                }
                break;
            }
            default: {
                size_t cs = cache_schema_tokens.size();
                for (size_t i = current_task->n_pos_cache - cs; i < current_task->prompt_tokens.size(); i++) {
                    llama_batch_add(batch, current_task->prompt_tokens[i], cs + i, {0}, false);
                }
                if (batch.n_tokens > 0) {
                    batch.logits[batch.n_tokens - 1] = true;
                }
                break;
            }
        }

        // push the non-cached/uncommon remainder of the prompt into the sampler
        if (current_task->type != TASK_SET_SCHEMA) {
            for (size_t i = cache_prompt_tokens.size(); i < current_task->prompt_tokens.size(); i++) {
                llama_token t = current_task->prompt_tokens[i];

                sampler s;
                if (cache_sampling_states.size()) {
                    s = cache_sampling_states.back();
                }
                sampler_result sres = s.add(ctx, t);
                if (sres == SAMPLER_RESULT_FINISH) {
                    // we've come to end of sampler grammar
                    spdlog::info("run {}: prompt syntax finished", current_task->id);
                    current_task->state = STATE_DONE;
                    return;
                } else if (sres == SAMPLER_RESULT_ERROR) {
                    // we've come across an invalid token
                    spdlog::error("run {}: prompt contains syntax errors", current_task->id);
                    current_task->state = STATE_ERROR;
                    return;
                }
                cache_sampling_states.push_back(s);
            }
        }

        current_task->state = STATE_DECODE;
    }

    if (current_task->state != STATE_DECODE || batch.n_tokens == 0) {
        current_task->state = STATE_DONE;
        return;
    }

    for (int32_t i = current_task->n_pos; i < (int32_t)batch.n_tokens; i += n_batch) {
        int n_eval = std::min(batch.n_tokens - i, n_batch);
        const int ret = llama_decode(
            ctx, llama_batch_get_one(batch.token + i, n_eval, current_task->n_pos_cache + current_task->n_pos, 0));
        if (ret != 0) {
            spdlog::error("run {}: failed to decode the batch", current_task->id);
            current_task->state = STATE_ERROR;
            return;
        }

        // update cache
        switch (current_task->type) {
            case TASK_SET_SCHEMA:
                for (int32_t j = 0; j < n_eval; j++) {
                    cache_schema_tokens.push_back(batch.token[i + j]);
                }
                break;
            default:
                for (int32_t j = 0; j < n_eval; j++) {
                    llama_token t = batch.token[i + j];
                    cache_prompt_tokens.push_back(t);
                }
                break;
        }

        current_task->n_pos += n_eval;

        const size_t n_pos = current_task->n_pos_cache + current_task->n_pos;
        switch (current_task->type) {
            case TASK_SET_SCHEMA:
                if (n_pos < current_task->prompt_tokens.size()) {
                    continue;
                } else {
                    current_task->state = STATE_DONE;
                    return;
                }
            case TASK_PRIME:
                if (n_pos < cache_schema_tokens.size() + current_task->prompt_tokens.size()) {
                    continue;
                } else {
                    current_task->state = STATE_DONE;
                    return;
                }
            case TASK_AUTOCOMPLETE:
                if (n_pos < cache_schema_tokens.size() + current_task->prompt_tokens.size()) {
                    continue;
                }
                break;
        }

        sampler s;
        if (cache_sampling_states.size()) {
            s = cache_sampling_states.back();
        }

        bool valid, finished;
        llama_token tok;
        std::string tok_str;
        std::tie(valid, finished, tok, tok_str) = s.get(ctx);

        if (!valid) {
            current_task->state = STATE_ERROR;
            return;
        }

        if (tok == llama_token_eos(m)) {
            current_task->state = STATE_DONE;
            return;
        } else {
            task_result res;
            res.t = std::move(tok_str);

            std::unique_lock<std::mutex> rlock(mtx_results);
            results.insert(std::pair<uint, task_result>(current_task->id, res));
            rlock.unlock();
            c_results.notify_all();
        }

        if (finished) {
            current_task->state = STATE_DONE;
            return;
        }

        llama_batch_add(batch, tok, current_task->n_pos_cache + i, {0}, true);

        cache_sampling_states.push_back(s);

        current_task->n_decoded++;
    }

    if (current_task->n_decoded > 256) {
        current_task->state = STATE_DONE;
    }
}

std::vector<llama_token> model::tokenize(std::string &text) {
    int n_tokens = text.length() + 1;
    std::vector<llama_token> result(n_tokens);
    n_tokens = llama_tokenize(m, text.data(), text.length(), result.data(), result.size(), false, true);
    if (n_tokens > 0) {
        result.resize(n_tokens);
    }
    return result;
}

size_t model::common_part(std::vector<llama_token> &a, std::vector<llama_token> &b) {
    size_t i = 0;
    while (i < a.size() && i < b.size()) {
        if (a[i] != b[i]) {
            return i;
        }
        i++;
    }
    return i;
}
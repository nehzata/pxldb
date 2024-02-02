#ifndef LLM_MODEL_H
#define LLM_MODEL_H

#include <llama.h>

#include <map>
#include <mutex>
#include <string>
#include <vector>

// #include "common/sampling.h"
#include "sampler.h"

// clang-format off
enum task_type {
    TASK_SET_SCHEMA,
    TASK_PRIME,
    TASK_AUTOCOMPLETE
};
enum task_state {
    STATE_LOADING,
    STATE_DECODE,
    STATE_DONE,
    STATE_ERROR,
    STATE_CANCEL,
    STATE_STOP
};
// clang-format on

struct task {
    task(uint id, task_type type, uint schema_id, std::vector<llama_token> &prompt_tokens)
        : id(id), type(type), schema_id(schema_id), prompt_tokens(prompt_tokens) {}

    uint id;
    task_type type;
    uint schema_id;
    std::vector<llama_token> prompt_tokens;
    task_state state = STATE_LOADING;
    int n_len = 0;
    int n_pos_cache = 0;
    int n_pos = 0;
    int n_decoded = 0;
};

struct task_result {
    std::string t;
    bool stop = false;
    bool error = false;
    bool cancel = false;
};

class model {
   private:
    llama_model *m;
    llama_context *ctx;
    std::string mpath;
    int n_ctx;
    int n_batch;
    int n_threads;
    uint g_req_id;

    std::mutex mtx_queue;
    std::condition_variable c_queue;
    std::vector<task *> queue;
    uint current_schema_id;
    task *current_task;

    std::mutex mtx_results;
    std::condition_variable c_results;
    std::map<uint, task_result> results;

    llama_batch batch;
    std::vector<llama_token> cache_schema_tokens;
    std::vector<llama_token> cache_prompt_tokens;
    std::vector<sampler> cache_sampling_states;

    model();

    std::vector<llama_token> tokenize(std::string &prompt);
    size_t common_part(std::vector<llama_token> &a, std::vector<llama_token> &b);

   public:
    ~model();
    model(std::string mpath, int n_ctx, int n_batch, int n_threads);

    int init();

    void set_schema(uint schema_id, std::string &schema);
    uint prime(uint schema_id, std::string &prompt);
    uint autocomplete(uint schema_id, std::string &prompt);

    void run();
    bool next_result(uint task_id, task_result &r);
};

#endif
#include "sampler.h"

#include <spdlog/spdlog.h>

#include "common/common.h"

sampler::sampler() {}

bool sampler::add(llama_context *ctx, llama_token t) {
    const std::string t_str = llama_token_to_piece(ctx, t);

    const char *c = t_str.c_str();
    spdlog::debug("sampler:add({})", t_str);

    // make a scratch copy of grammar
    // to check token against
    grammar_pgsql g_tmp = g;
    buffer b_tmp = b;

    while (*c != 0) {
        b_tmp.add(*c);
        while (!b_tmp.empty()) {
            grammar_result r = g_tmp.eval(0, b_tmp);
            if (r.code != GRAMMAR_RESULT_CONTINUE) {
                // we've either hit a syntax error OR
                // grammar finish.
                // both are unexpected
                return false;
            }
            if (r.n_rewind != 0) {
                b_tmp.rewind(r.n_rewind);
            }
        }
        c++;
    }

    // update grammar to accepted value
    g = g_tmp;
    b = b_tmp;

    return true;
}

struct token {
    llama_token id;
    float logit;
};

const int top_k = 50;

std::tuple<bool, bool, llama_token, std::string> sampler::get(llama_context *ctx) {
    int n_vocab = llama_n_vocab(llama_get_model(ctx));
    float *logits = llama_get_logits_ith(ctx, 0);

    std::vector<token> candidates;
    for (llama_token t = 0; t < n_vocab; t++) {
        candidates.emplace_back(token{t, logits[t]});
    }

    // sort candidates
    std::sort(candidates.begin(), candidates.end(), [](const token &a, const token &b) { return a.logit > b.logit; });

    for (int i = 0; i < top_k; i++) {
        llama_token t = candidates[i].id;
        const std::string t_str = llama_token_to_piece(ctx, t);

        const char *c = t_str.c_str();

        spdlog::debug("sampler:trying <{}>", t_str);

        // make a scratch copy of grammar & buffer
        // to check token against
        grammar_pgsql g_tmp = g;
        buffer b_tmp = b;

        bool finished = false;
        while (*c != 0) {
            b_tmp.add(*c);
            while (!b_tmp.empty()) {
                grammar_result r = g_tmp.eval(0, b_tmp);
                if (r.code == GRAMMAR_RESULT_ERROR) {
                    goto next;
                }
                if (r.n_rewind != 0) {
                    b_tmp.rewind(r.n_rewind);
                }
                if (r.code != GRAMMAR_RESULT_CONTINUE) {
                    finished = r.code == GRAMMAR_RESULT_FINISH;
                    goto end;
                }
            }
            c++;
            continue;
        end:
            break;
        }

        // update grammar to accepted value
        g = g_tmp;
        b = b_tmp;

        return {true, finished, t, t_str};

    next:
        continue;
    }

    spdlog::debug("sampler:nothing found");

    return {false, false, -1, ""};
}
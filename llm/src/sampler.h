#ifndef LLM_SAMPLING_H
#define LLM_SAMPLING_H

#include <string>
#include <tuple>

#include "grammar/buffer.h"
#include "grammar/pgsql/pgsql.h"
#include "llama.h"

// clang-format off
enum sampler_result {
    SAMPLER_RESULT_ERROR,
    SAMPLER_RESULT_CONTINUE,
    SAMPLER_RESULT_FINISH,
};
// clang-format on

class sampler {
   private:
    grammar_pgsql g;
    buffer b;

   public:
    sampler();

    // location in the grammar
    sampler_result add(llama_context *ctx, llama_token t);
    std::tuple<bool, bool, llama_token, std::string> get(llama_context *ctx);
};

#endif
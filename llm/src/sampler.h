#ifndef LLM_SAMPLING_H
#define LLM_SAMPLING_H

#include <string>
#include <tuple>

#include "grammar/buffer.h"
#include "grammar/pgsql/pgsql.h"
#include "llama.h"

class sampler {
   private:
    grammar_pgsql g;
    buffer b;

   public:
    sampler();

    // location in the grammar
    bool add(llama_context *ctx, llama_token t);
    std::tuple<bool, bool, llama_token, std::string> get(llama_context *ctx);
};

#endif
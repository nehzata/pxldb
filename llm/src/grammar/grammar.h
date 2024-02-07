#ifndef LLM_GRAMMAR_H
#define LLM_GRAMMAR_H

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <memory>

#include "buffer.h"

// clang-format off
enum grammar_result_code {
    GRAMMAR_RESULT_ERROR,
    GRAMMAR_RESULT_CONTINUE,
    GRAMMAR_RESULT_FINISH,
};
// clang-format on

// struct grammar_result {
//     grammar_result_code code;
//     size_t n_consumed;
// };

class grammar {
   public:
    typedef std::function<grammar *()> factory;

   public:
    virtual ~grammar() {}
    virtual grammar_result_code eval(uint depth, buffer &b) = 0;

    virtual std::unique_ptr<grammar> clone() const = 0;
};

#endif
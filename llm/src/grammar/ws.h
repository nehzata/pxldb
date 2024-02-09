#ifndef LLM_GRAMMAR_WS_H
#define LLM_GRAMMAR_WS_H

#include "max_n.h"
#include "regex.h"

class grammar_ws : public grammar_max_n {
   public:
    virtual ~grammar_ws() {}
    // clang-format off
    grammar_ws() : grammar_max_n(
        []() { return new grammar_regex(R"([\n\t\s])"); },
        5
    ) {}
    //clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_ws(*this));
    }
};

#endif
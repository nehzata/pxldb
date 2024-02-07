#ifndef LLM_GRAMMAR_EPSILON_H
#define LLM_GRAMMAR_EPSILON_H

#include <spdlog/spdlog.h>

#include "grammar.h"

class grammar_epsilon : public grammar {
   public:
    virtual ~grammar_epsilon() {}
    grammar_epsilon() {}

    grammar_result_code eval(uint depth, buffer &) override {
        spdlog::debug("{} epsilon", std::string(depth, ' '));
        // return {GRAMMAR_RESULT_FINISH, 0};
        return GRAMMAR_RESULT_FINISH;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_epsilon(*this));
    }
};

#endif
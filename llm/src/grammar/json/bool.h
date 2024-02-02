#ifndef LLM_GRAMMAR_JSON_BOOL_H
#define LLM_GRAMMAR_JSON_BOOL_H

#include "grammar/alternatives.h"
#include "grammar/identifier.h"

class grammar_json_bool : public grammar_alternatives {
   public:
    virtual ~grammar_json_bool() {}

    // clang-format off
    grammar_json_bool() : grammar_alternatives({
        new grammar_identifier("true"),
        new grammar_identifier("false")
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_json_bool(*this));
    }
};

#endif
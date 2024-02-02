#ifndef LLM_GRAMMAR_JSON_NULL_H
#define LLM_GRAMMAR_JSON_NULL_H

#include "grammar/identifier.h"

class grammar_json_null : public grammar_identifier {
   public:
    virtual ~grammar_json_null() {}
    grammar_json_null() : grammar_identifier("null") {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_json_null(*this));
    }
};

#endif
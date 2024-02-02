#ifndef LLM_GRAMMAR_JSON_ARRAY_H
#define LLM_GRAMMAR_JSON_ARRAY_H

#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "json.h"

class grammar_json_array : public grammar_list {
   public:
    virtual ~grammar_json_array() {}

    // clang-format off
    grammar_json_array() : grammar_list({
        new grammar_identifier("["),
        new grammar_zero_or_one(
            new grammar_list({
                new grammar_zero_or_one(new grammar_ws()),
                new grammar_json(),
                new grammar_zero_or_many([]() {
                    return new grammar_list({
                        new grammar_identifier(","),
                        new grammar_zero_or_one(new grammar_ws()),
                        new grammar_json()
                    });
                })
            })
        ),
        new grammar_identifier("]")
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_json_array(*this));
    }
};

#endif
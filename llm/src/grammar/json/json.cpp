#include "json.h"

#include "array.h"
#include "bool.h"
#include "null.h"
#include "number.h"
#include "object.h"
#include "string.h"

grammar_json::~grammar_json() {}

grammar_json::grammar_json() {}

grammar_result_code grammar_json::eval(uint depth, buffer &b) {
    if (!g) {
        // clang-format off
        g = std::unique_ptr<grammar>(
            new grammar_alternatives({
                new grammar_json_null(),
                new grammar_json_number(),
                new grammar_json_bool(),
                new grammar_json_string(),
                new grammar_json_object(),
                new grammar_json_array()
            })
        );
        // clang-format on
    }
    return g->eval(depth + 1, b);
}

grammar_json::grammar_json(const grammar_json &src) : grammar(src) {
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
}

grammar_json &grammar_json::operator=(const grammar_json &src) {
    if (this == &src) {
        return *this;
    }
    grammar::operator=(src);
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
    return *this;
}
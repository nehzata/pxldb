#ifndef LLM_GRAMMAR_PGSQL_LITERAL_H
#define LLM_GRAMMAR_PGSQL_LITERAL_H

#include "grammar/alternatives.h"
#include "grammar/identifier_ci.h"
#include "number.h"
#include "string.h"

class grammar_pgsql_literal : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_literal() {}
    // clang-format off
    grammar_pgsql_literal() : grammar_alternatives({
        new grammar_pgsql_number(),
        new grammar_pgsql_string(),
        new grammar_identifier_ci("NULL"),
        new grammar_identifier_ci("TRUE"),
        new grammar_identifier_ci("FALSE"),
        new grammar_identifier_ci("CURRENT_TIME"),
        new grammar_identifier_ci("CURRENT_DATE"),
        new grammar_identifier_ci("CURRENT_TIMESTAMP")
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_literal(*this));
    }
};

#endif
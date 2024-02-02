#ifndef LLM_GRAMMAR_PGSQL_RESULT_COLUMN_H
#define LLM_GRAMMAR_PGSQL_RESULT_COLUMN_H

#include "expr.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/list.h"
#include "identifier.h"

class grammar_pgsql_result_column : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_result_column() {}
    // clang-format off
    grammar_pgsql_result_column() : grammar_alternatives({
      new grammar_identifier("*"),
      new grammar_list({
        new grammar_pgsql_identifier(), // table_name
        new grammar_identifier(".*")
      }),
      new grammar_pgsql_expr()
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_result_column(*this));
    }
};

#endif
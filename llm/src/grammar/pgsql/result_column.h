#ifndef LLM_GRAMMAR_PGSQL_RESULT_COLUMN_H
#define LLM_GRAMMAR_PGSQL_RESULT_COLUMN_H

#include "column_name.h"
#include "expr.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_one.h"
#include "identifier.h"
#include "keyword.h"
#include "table_name.h"

class grammar_pgsql_result_column : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_result_column() {}
    // clang-format off
    grammar_pgsql_result_column() : grammar_alternatives({
      new grammar_identifier("*"),
      new grammar_list({
        new grammar_pgsql_table_name(),
        new grammar_identifier(".*")
      }),
      new grammar_list({
        new grammar_pgsql_expr(),
        new grammar_zero_or_one(
          new grammar_alternatives({
            new grammar_list({
              new grammar_ws(),
              new grammar_identifier_ci("AS"),
              new grammar_ws(),
              new grammar_pgsql_column_label(),
            }),
            new grammar_list({
              new grammar_ws(),
              new grammar_pgsql_column_label_bare(),
            }),
          })
        )
      }),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_result_column(*this));
    }
};

#endif
#ifndef LLM_GRAMMAR_PGSQL_FUNCTION_ARGUMENTS_H
#define LLM_GRAMMAR_PGSQL_FUNCTION_ARGUMENTS_H

#include "expr.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "order_by.h"
#include "result_column.h"
#include "table_or_subquery.h"

class grammar_pgsql_function_arguments : public grammar_zero_or_one {
   public:
    virtual ~grammar_pgsql_function_arguments() {}
    // clang-format off
    grammar_pgsql_function_arguments() : grammar_zero_or_one(
      new grammar_alternatives({
        new grammar_list({
          new grammar_zero_or_one(
            new grammar_list({
              new grammar_identifier_ci("DISTINCT"),
              new grammar_ws(),
            })
          ),
          new grammar_pgsql_expr(),
          new grammar_zero_or_many([]() {
            return new grammar_list({
              new grammar_identifier(","),
              new grammar_zero_or_one(new grammar_ws()),
              new grammar_pgsql_expr()
            });
          }),
          new grammar_zero_or_one(
            new grammar_pgsql_order_by()
          )
        }),
        new grammar_identifier("*")
      })
     ) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_function_arguments(*this));
    }
};

#endif
#ifndef LLM_GRAMMAR_PGSQL_ORDER_BY_H
#define LLM_GRAMMAR_PGSQL_ORDER_BY_H

#include "expr.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "result_column.h"
#include "table_or_subquery.h"

class grammar_pgsql_ordering_term : public grammar_list {
   public:
    virtual ~grammar_pgsql_ordering_term() {}
    // clang-format off
    grammar_pgsql_ordering_term() : grammar_list({
      new grammar_pgsql_expr(),
      new grammar_zero_or_one(
        new grammar_list({
          new grammar_ws(),
          new grammar_alternatives({
            new grammar_identifier_ci("ASC"),
            new grammar_identifier_ci("DESC")
          })
        })
      ),
      new grammar_zero_or_one(
        new grammar_alternatives({
          new grammar_identifier_ci(" NULLS FIRST"),
          new grammar_identifier_ci(" NULLS LAST")
        })
      ),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_ordering_term(*this));
    }
};

class grammar_pgsql_order_by : public grammar_list {
   public:
    virtual ~grammar_pgsql_order_by() {}
    // clang-format off
    grammar_pgsql_order_by() : grammar_list({
      new grammar_identifier_ci("ORDER BY"),
      new grammar_ws(),
      new grammar_pgsql_ordering_term(),
      new grammar_zero_or_many([](){
        return new grammar_list({
          new grammar_identifier(","),
          new grammar_zero_or_one(new grammar_ws()),
          new grammar_pgsql_ordering_term()
        });
      })
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_order_by(*this));
    }
};

#endif
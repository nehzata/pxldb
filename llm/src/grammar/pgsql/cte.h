#ifndef LLM_GRAMMAR_PGSQL_CTE_H
#define LLM_GRAMMAR_PGSQL_CTE_H

#include "expr.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "identifier.h"
#include "select.h"

class grammar_pgsql_cte : public grammar_list {
   public:
    virtual ~grammar_pgsql_cte() {}
    // clang-format off
    grammar_pgsql_cte() : grammar_list({
      new grammar_pgsql_identifier(),
      new grammar_zero_or_one(
        new grammar_list({
          new grammar_ws(),
          new grammar_identifier("("),
          new grammar_pgsql_identifier(),
          new grammar_zero_or_many([]() {
            return new grammar_list({
              new grammar_identifier(","),
              new grammar_ws(),
              new grammar_pgsql_identifier()
            });
          }),
          new grammar_identifier(")"),
        })
      ),
      new grammar_ws(),
      new grammar_identifier_ci("AS"),
      new grammar_ws(),
      new grammar_identifier("("),
      new grammar_pgsql_select(),
      new grammar_identifier(")"),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_cte(*this));
    }
};

#endif
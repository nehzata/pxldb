#ifndef LLM_GRAMMAR_PGSQL_JOIN_CLAUSE_H
#define LLM_GRAMMAR_PGSQL_JOIN_CLAUSE_H

#include "expr.h"
#include "grammar/alternatives.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "result_column.h"
#include "table_or_subquery.h"

class grammar_pgsql_join_operator : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_join_operator() {}
    // clang-format off
    grammar_pgsql_join_operator() : grammar_alternatives({
      new grammar_identifier_ci("LEFT JOIN"),
      new grammar_identifier_ci("LEFT OUTER JOIN"),
      new grammar_identifier_ci("NATURAL LEFT JOIN"),
      new grammar_identifier_ci("NATURAL LEFT OUTER JOIN"),
      new grammar_identifier_ci("RIGHT JOIN"),
      new grammar_identifier_ci("RIGHT OUTER JOIN"),
      new grammar_identifier_ci("NATURAL RIGHT JOIN"),
      new grammar_identifier_ci("NATURAL RIGHT OUTER JOIN"),
      new grammar_identifier_ci("FULL JOIN"),
      new grammar_identifier_ci("FULL OUTER JOIN"),
      new grammar_identifier_ci("NATURAL FULL JOIN"),
      new grammar_identifier_ci("NATURAL FULL OUTER JOIN"),
      new grammar_identifier_ci("INNER JOIN"),
      new grammar_identifier_ci("NATURAL INNER JOIN"),
      new grammar_identifier_ci("CROSS JOIN"),
      new grammar_identifier_ci("JOIN"),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_join_operator(*this));
    }
};

class grammar_pgsql_join_clause_inner : public grammar_list {
   public:
    virtual ~grammar_pgsql_join_clause_inner() {}
    // clang-format off
    grammar_pgsql_join_clause_inner() : grammar_list({
      new grammar_pgsql_join_operator(),
      new grammar_ws(),
      new grammar_pgsql_table_or_subquery(),
      new grammar_ws(),
      new grammar_identifier_ci("ON"),
      new grammar_ws(),
      new grammar_pgsql_expr()
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_join_clause_inner(*this));
    }
};

class grammar_pgsql_join_clause : public grammar_list {
   public:
    virtual ~grammar_pgsql_join_clause() {}
    // clang-format off
    grammar_pgsql_join_clause() : grammar_list({
      new grammar_pgsql_join_clause_inner(),
      new grammar_zero_or_many([]() {
        return new grammar_list({
          new grammar_ws(),
          new grammar_pgsql_join_clause_inner()
        });
      })
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_join_clause(*this));
    }
};

#endif

#include "table_or_subquery.h"

#include <spdlog/spdlog.h>

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

grammar_pgsql_table_or_subquery::~grammar_pgsql_table_or_subquery() {}

grammar_pgsql_table_or_subquery::grammar_pgsql_table_or_subquery() {}

grammar_result_code grammar_pgsql_table_or_subquery::eval(uint depth, buffer &b) {
    if (!g) {
        // clang-format off
        g = std::unique_ptr<grammar>(
            new grammar_alternatives({
              new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#table-or-subquery:~:text=table%2Dor%2Dsubquery%3A-,schema%2Dname,table%2Dalias,-INDEXED
                new grammar_pgsql_identifier(),
                new grammar_zero_or_one(
                  new grammar_list({
                    new grammar_zero_or_one(
                      new grammar_list({
                        new grammar_ws(),
                        new grammar_identifier_ci("AS")
                      })
                    ),
                    new grammar_ws(),
                    new grammar_pgsql_identifier()
                  })
                )
              }),
              new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#table-or-subquery:~:text=table%2Dfunction%2Dname,)
                new grammar_pgsql_identifier(),
                new grammar_identifier("("),
                new grammar_pgsql_expr(),
                new grammar_zero_or_many([]() {
                  return new grammar_list({
                    new grammar_identifier(","),
                    new grammar_zero_or_one(new grammar_ws()),
                    new grammar_pgsql_expr()
                  });
                }),
                new grammar_identifier(")"),
                new grammar_list({
                  new grammar_ws(),
                  new grammar_zero_or_one(
                    new grammar_identifier_ci("AS")
                  ),
                  new grammar_ws(),
                  new grammar_pgsql_identifier(),
                })
              }),
              new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#table-or-subquery:~:text=table%2Dalias-,(,),-(
                new grammar_identifier("("),
                new grammar_pgsql_select(),
                new grammar_identifier(")"),
                new grammar_zero_or_one(
                  new grammar_list({
                    new grammar_ws(),
                    new grammar_identifier_ci("AS"),
                    new grammar_ws(),
                    new grammar_pgsql_identifier()
                  })
                )
              }),
              new grammar_list({
                new grammar_identifier("("),
                new grammar_pgsql_table_or_subquery(), // table name
                new grammar_zero_or_many([]() {
                  return new grammar_list({
                    new grammar_identifier(","),
                    new grammar_ws(),
                    new grammar_pgsql_table_or_subquery() // table name
                  });
                }),
                new grammar_identifier(")")
              })
            })
        );
        // clang-format on
    }
    grammar_result_code r = g->eval(depth + 1, b);
    spdlog::debug("{} pgsql_table_or_subquery: {}", std::string(depth, ' '), int(r));
    return r;
}

grammar_pgsql_table_or_subquery::grammar_pgsql_table_or_subquery(const grammar_pgsql_table_or_subquery &src)
    : grammar(src) {
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
}

grammar_pgsql_table_or_subquery &grammar_pgsql_table_or_subquery::operator=(
    const grammar_pgsql_table_or_subquery &src) {
    if (this == &src) {
        return *this;
    }
    grammar::operator=(src);
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
    return *this;
}
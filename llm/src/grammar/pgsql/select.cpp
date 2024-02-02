
#include "select.h"

#include <spdlog/spdlog.h>

#include "cte.h"
#include "grammar/alternatives.h"
#include "grammar/epsilon.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/one_or_more.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "ordering_term.h"
#include "select_core.h"

grammar_pgsql_select::~grammar_pgsql_select() {}

grammar_pgsql_select::grammar_pgsql_select() {}

grammar_result grammar_pgsql_select::eval(uint depth, buffer &b) {
    if (!g) {
        // clang-format off
        g = std::unique_ptr<grammar>(
          new grammar_list({
            new grammar_zero_or_one(new grammar_ws()),
            new grammar_zero_or_one(
              new grammar_list({
                new grammar_identifier_ci("WITH"),
                new grammar_zero_or_one(
                  new grammar_list({
                    new grammar_ws(),
                    new grammar_identifier_ci("RECURSIVE")
                  })
                ),
                new grammar_ws(),
                new grammar_pgsql_cte(),
                new grammar_zero_or_many([]() {
                  return new grammar_list({
                    new grammar_identifier(","),
                    new grammar_zero_or_one(new grammar_ws()),
                    new grammar_pgsql_cte()
                  });
                }),
                new grammar_zero_or_one(new grammar_ws()),
              })
            ),
            new grammar_pgsql_select_core(),
            new grammar_alternatives({
              // new grammar_list({
              //   new grammar_pgsql_compound_operator(),
              //   new grammar_pgsql_select_core()
              // }),
              new grammar_list({
                new grammar_zero_or_one(
                  new grammar_list({
                    new grammar_ws(),
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
                  })
                ),
                new grammar_zero_or_one(
                  new grammar_list({
                    new grammar_ws(),
                    new grammar_identifier_ci("LIMIT"),
                    new grammar_ws(),
                    new grammar_pgsql_expr(),
                    new grammar_zero_or_many([](){
                      return new grammar_list({
                        new grammar_identifier(","),
                        new grammar_zero_or_one(new grammar_ws()),
                        new grammar_pgsql_expr()
                      });
                    }),
                    new grammar_zero_or_one(
                      new grammar_list({
                        new grammar_ws(),
                        new grammar_identifier_ci("OFFSET"),
                        new grammar_ws(),
                        new grammar_pgsql_expr()
                      })
                    )
                  })
                ),
              })
            })
          })
        );
        // clang-format on
    }
    grammar_result r = g->eval(depth + 1, b);
    spdlog::debug("{} pgsql_select: {},{}", std::string(depth, ' '), int(r.code), r.n_rewind);
    return r;
}

grammar_pgsql_select::grammar_pgsql_select(const grammar_pgsql_select &src) : grammar(src) {
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
}

grammar_pgsql_select &grammar_pgsql_select::operator=(const grammar_pgsql_select &src) {
    if (this == &src) {
        return *this;
    }
    grammar::operator=(src);
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
    return *this;
}
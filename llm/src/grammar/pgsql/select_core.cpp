
#include "select_core.h"

#include <spdlog/spdlog.h>

#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/one_or_more.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "identifier.h"
#include "join_clause.h"
#include "result_column.h"
#include "table_or_subquery.h"

class grammar_pgsql_value : public grammar_list {
   public:
    virtual ~grammar_pgsql_value() {}
    // clang-format off
    grammar_pgsql_value() : grammar_list({
      new grammar_identifier("("),
      new grammar_pgsql_expr(),
      new grammar_zero_or_many([]() {
        return new grammar_list({
          new grammar_identifier(","),
          new grammar_zero_or_one(new grammar_ws()),
          new grammar_pgsql_expr()
        });
      }),
      new grammar_identifier(")")
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_value(*this));
    }
};

grammar_pgsql_select_core::~grammar_pgsql_select_core() {}

grammar_pgsql_select_core::grammar_pgsql_select_core() {}

grammar_result_code grammar_pgsql_select_core::eval(uint depth, buffer &b) {
    if (!g) {
        // clang-format off
        g = std::unique_ptr<grammar>(
          new grammar_alternatives({
            new grammar_list({
              new grammar_identifier_ci("SELECT"),
              new grammar_zero_or_one(
                new grammar_list({
                  new grammar_ws(),
                  new grammar_alternatives({
                    new grammar_identifier_ci("DISTINCT"),
                    new grammar_identifier_ci("ALL")
                  }),
                })
              ),
              new grammar_ws(),
              new grammar_pgsql_result_column(),
              new grammar_zero_or_many([]() {
                return new grammar_list({
                  new grammar_identifier(","),
                  new grammar_zero_or_one(new grammar_ws()),
                  new grammar_pgsql_result_column()
                });
              }),
              new grammar_zero_or_one(
                new grammar_list({
                  new grammar_ws(),
                  new grammar_identifier_ci("FROM"),
                  new grammar_ws(),
                  new grammar_pgsql_table_or_subquery(), // table name
                  new grammar_zero_or_one(
                    new grammar_alternatives({
                      new grammar_one_or_more([](){
                        return new grammar_list({
                          new grammar_identifier(","),
                          new grammar_zero_or_one(new grammar_ws()),
                          new grammar_pgsql_table_or_subquery() // table name
                        });
                      }),
                      new grammar_list({
                        new grammar_ws(),
                        new grammar_pgsql_join_clause()
                      })
                    })
                  )
                })
              ),
              new grammar_zero_or_one(
                new grammar_list({
                  new grammar_ws(),
                  new grammar_identifier_ci("WHERE"),
                  new grammar_ws(),
                  new grammar_pgsql_expr()
                })
              ),
            }),
            new grammar_list({
              new grammar_identifier_ci("VALUES"),
              new grammar_ws(),
              new grammar_pgsql_value(),
              new grammar_zero_or_many([]() {
                return new grammar_list({
                  new grammar_identifier(","),
                  new grammar_zero_or_one(new grammar_ws()),
                  new grammar_pgsql_value()
                });
              })
            })
          })
        );
        // clang-format on
    }
    grammar_result_code r = g->eval(depth + 1, b);
    spdlog::debug("{} pgsql_select_core: {}", std::string(depth, ' '), int(r));
    return r;
}

grammar_pgsql_select_core::grammar_pgsql_select_core(const grammar_pgsql_select_core &src) : grammar(src) {
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
}

grammar_pgsql_select_core &grammar_pgsql_select_core::operator=(const grammar_pgsql_select_core &src) {
    if (this == &src) {
        return *this;
    }
    grammar::operator=(src);
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
    return *this;
}
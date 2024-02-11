#ifndef LLM_GRAMMAR_PGSQL_TYPENAME_H
#define LLM_GRAMMAR_PGSQL_TYPENAME_H

#include "expr.h"
#include "function.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "number.h"

class grammar_pgsql_array : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_array() {}
    // clang-format off
    grammar_pgsql_array() : grammar_alternatives({
        new grammar_identifier_ci("[]"),
        new grammar_list({
          new grammar_identifier_ci("["),
          new grammar_pgsql_number(),
          new grammar_identifier_ci("]")
        })
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_array(*this));
    }
};

class grammar_pgsql_typename : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_typename() {}
    // clang-format off
    grammar_pgsql_typename() : grammar_alternatives({
      new grammar_list({
        new grammar_pgsql_function_name(),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_identifier("("),
            new grammar_zero_or_one(
              new grammar_list({
                new grammar_pgsql_expr(),
                new grammar_zero_or_many([]() {
                  return new grammar_list({
                    new grammar_identifier(","),
                    new grammar_zero_or_one(new grammar_ws()),
                    new grammar_pgsql_expr()
                  });
                })
              })
            ),
            new grammar_identifier(")")
          })
        )
      }),
      new grammar_identifier_ci("INTEGER"),
      new grammar_identifier_ci("INT"),
      new grammar_identifier_ci("SMALLINT"),
      new grammar_identifier_ci("BIGINT"),
      new grammar_identifier_ci("REAL"),
      new grammar_identifier_ci("FLOAT"),
      new grammar_identifier_ci("DOUBLE PRECISION"),
      new grammar_identifier_ci("DECIMAL"),
      new grammar_identifier_ci("DEC"),
      new grammar_identifier_ci("NUMERIC"),
      new grammar_identifier_ci("BOOLEAN"),
      new grammar_list({
        new grammar_identifier_ci("BIT"),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_ws(),
            new grammar_identifier_ci("VARYING"),
          })
        ),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_identifier("("),
            new grammar_zero_or_one(
              new grammar_list({
                new grammar_pgsql_expr(),
                new grammar_zero_or_many([]() {
                  return new grammar_list({
                    new grammar_identifier(","),
                    new grammar_zero_or_one(new grammar_ws()),
                    new grammar_pgsql_expr()
                  });
                })
              })
            ),
            new grammar_identifier(")")
          })
        )
      }),
      new grammar_list({
        new grammar_alternatives({
          new grammar_list({
            new grammar_alternatives({
              new grammar_identifier_ci("CHARACTER"),
              new grammar_identifier_ci("CHAR"),
            }),
            new grammar_zero_or_one(
              new grammar_list({
                new grammar_ws(),
                new grammar_identifier_ci("VARYING"),
              })
            ),
          }),
          new grammar_identifier_ci("VARCHAR"),
        }),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_identifier("("),
            new grammar_pgsql_number(),
            new grammar_identifier(")")
          })
        )
      }),
      new grammar_list({
        new grammar_identifier_ci("TIMESTAMP("),
        new grammar_pgsql_number(),
        new grammar_identifier(")"),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_ws(),
            new grammar_alternatives({
              new grammar_identifier_ci("WITHOUT"),
              new grammar_identifier_ci("WITH")
            }),
            new grammar_ws(),
            new grammar_identifier_ci("TIME ZONE"),
          })
        )
      }),
      new grammar_list({
        new grammar_identifier_ci("TIMESTAMP"),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_ws(),
            new grammar_alternatives({
              new grammar_identifier_ci("WITHOUT"),
              new grammar_identifier_ci("WITH")
            }),
            new grammar_ws(),
            new grammar_identifier_ci("TIME ZONE"),
          })
        )
      }),
      new grammar_list({
        new grammar_identifier_ci("TIME("),
        new grammar_pgsql_number(),
        new grammar_identifier(")"),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_ws(),
            new grammar_alternatives({
              new grammar_identifier_ci("WITHOUT"),
              new grammar_identifier_ci("WITH")
            }),
            new grammar_ws(),
            new grammar_identifier_ci("TIME ZONE"),
          })
        )
      }),
      new grammar_list({
        new grammar_identifier_ci("TIME"),
        new grammar_zero_or_one(
          new grammar_list({
            new grammar_ws(),
            new grammar_alternatives({
              new grammar_identifier_ci("WITHOUT"),
              new grammar_identifier_ci("WITH")
            }),
            new grammar_ws(),
            new grammar_identifier_ci("TIME ZONE"),
          })
        )
      }),
      // interval types
      // json types
      new grammar_zero_or_one(new grammar_pgsql_array())
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_typename(*this));
    }
};

#endif
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

class grammar_pgsql_function_name : public grammar_alternatives {
   private:
    std::string str;

   public:
    virtual ~grammar_pgsql_function_name() {}
    // clang-format off
    grammar_pgsql_function_name() : grammar_alternatives({
      new grammar_pgsql_identifier(),
      new grammar_pgsql_keyword_unreserved(),
      new grammar_pgsql_keyword_func_name(),
    }) {}
    // clang-format on

    // grammar_result_code eval(uint depth, buffer &b) override {
    //     const std::pair<bool, char> v = b.next();
    //     if (!v.first) {
    //         spdlog::debug("{} pgsql_function_name: {},null", std::string(depth, ' '), str);
    //         // return {str.size() ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR, 0};
    //         return str.size() ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR;
    //     }

    //     const char c = v.second;
    //     spdlog::debug("{} pgsql_function_name: {},{}", std::string(depth, ' '), str, c);
    //     if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') {
    //         str.push_back(c);
    //         // return {GRAMMAR_RESULT_CONTINUE, 0};
    //         return GRAMMAR_RESULT_CONTINUE;
    //     } else if (str.length()) {
    //         b.rewind(1);
    //         return GRAMMAR_RESULT_FINISH;
    //     }
    //     // return {GRAMMAR_RESULT_ERROR, 0};
    //     return GRAMMAR_RESULT_ERROR;
    // }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_function_name(*this));
    }
};

#endif
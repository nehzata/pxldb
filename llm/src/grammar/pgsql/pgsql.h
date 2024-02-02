#ifndef LLM_GRAMMAR_PGSQL_H
#define LLM_GRAMMAR_PGSQL_H

#include "comment.h"
#include "grammar/alternatives.h"
#include "grammar/identifier.h"
#include "grammar/list.h"
#include "grammar/regex.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "select.h"

class grammar_pgsql : public grammar_pgsql_comment {
   public:
    virtual ~grammar_pgsql() {}
    // clang-format off
    grammar_pgsql() : grammar_pgsql_comment(
      new grammar_alternatives({
        new grammar_pgsql_select()
      })
    ) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql(*this));
    }
};

// class grammar_pgsql : public grammar_list {
//    public:
//     virtual ~grammar_pgsql() {}
//     // clang-format off
//     grammar_pgsql() : grammar_list({
//       new grammar_zero_or_one(new grammar_ws()),
//       new grammar_zero_or_many([]{
//         return new grammar_list({
//           new grammar_identifier("--"),
//           new grammar_zero_or_many([]() { return new grammar_identifier(" "); }),
//           new grammar_zero_or_many([]() { return new grammar_regex(std::regex(R"([^\n])")); }),
//           new grammar_identifier("\n")
//         });
//       }),
//       new grammar_alternatives({
//         new grammar_pgsql_select()
//       })
//     }) {}
//     // clang-format on

//     virtual std::unique_ptr<grammar> clone() const override {
//         return std::unique_ptr<grammar>(new grammar_pgsql(*this));
//     }
// };

#endif
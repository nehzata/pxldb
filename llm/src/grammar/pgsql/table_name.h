#ifndef LLM_GRAMMAR_PGSQL_TABLE_NAME_H
#define LLM_GRAMMAR_PGSQL_TABLE_NAME_H

#include "grammar/alternatives.h"
#include "identifier.h"
#include "keyword.h"

class grammar_pgsql_table_name : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_table_name() {}
    // clang-format off
    grammar_pgsql_table_name() : grammar_alternatives({
      new grammar_pgsql_identifier(),
      new grammar_pgsql_keyword_unreserved(),
      new grammar_pgsql_keyword_col_name(),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_table_name(*this));
    }
};

#endif
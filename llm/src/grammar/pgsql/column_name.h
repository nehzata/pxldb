#ifndef LLM_GRAMMAR_PGSQL_COLUMN_NAME_H
#define LLM_GRAMMAR_PGSQL_COLUMN_NAME_H

#include "grammar/alternatives.h"
#include "identifier.h"
#include "keyword.h"

class grammar_pgsql_column_id : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_column_id() {}
    // clang-format off
    grammar_pgsql_column_id() : grammar_alternatives({
      new grammar_pgsql_identifier(),
      new grammar_pgsql_keyword_unreserved(),
      new grammar_pgsql_keyword_col_name(),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_column_id(*this));
    }
};

class grammar_pgsql_column_label : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_column_label() {}
    // clang-format off
    grammar_pgsql_column_label() : grammar_alternatives({
      new grammar_pgsql_identifier(),
      new grammar_pgsql_keyword_unreserved(),
      new grammar_pgsql_keyword_col_name(),
      new grammar_pgsql_keyword_func_name(),
      new grammar_pgsql_keyword_reserved(),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_column_label(*this));
    }
};

class grammar_pgsql_column_label_bare : public grammar_alternatives {
   public:
    virtual ~grammar_pgsql_column_label_bare() {}
    // clang-format off
    grammar_pgsql_column_label_bare() : grammar_alternatives({
      new grammar_pgsql_identifier(),
      new grammar_pgsql_keyword_bare_label(),
    }) {}
    // clang-format on

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_column_label_bare(*this));
    }
};

#endif
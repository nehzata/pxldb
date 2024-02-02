#ifndef LLM_GRAMMAR_PGSQL_TABLE_OR_SUBQUERY_H
#define LLM_GRAMMAR_PGSQL_TABLE_OR_SUBQUERY_H

#include <memory>

#include "grammar/grammar.h"

class grammar_pgsql_table_or_subquery : public grammar {
   private:
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_pgsql_table_or_subquery();
    grammar_pgsql_table_or_subquery();

    virtual grammar_result eval(uint depth, buffer &b) override;

    grammar_pgsql_table_or_subquery(const grammar_pgsql_table_or_subquery &);
    grammar_pgsql_table_or_subquery &operator=(const grammar_pgsql_table_or_subquery &);

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_table_or_subquery(*this));
    }
};

#endif
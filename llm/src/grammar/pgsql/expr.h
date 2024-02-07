#ifndef LLM_GRAMMAR_PGSQL_EXPR_H
#define LLM_GRAMMAR_PGSQL_EXPR_H

#include <list>
#include <memory>

#include "grammar/grammar.h"

class grammar_pgsql_expr : public grammar {
   private:
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_pgsql_expr();
    grammar_pgsql_expr();

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    grammar_pgsql_expr(const grammar_pgsql_expr &);
    grammar_pgsql_expr &operator=(const grammar_pgsql_expr &);

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_expr(*this));
    }
};

#endif
#ifndef LLM_GRAMMAR_PGSQL_SELECT_H
#define LLM_GRAMMAR_PGSQL_SELECT_H

#include <memory>

#include "grammar/grammar.h"

class grammar_pgsql_select : public grammar {
   private:
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_pgsql_select();
    grammar_pgsql_select();

    virtual grammar_result eval(uint depth, buffer &b) override;

    grammar_pgsql_select(const grammar_pgsql_select &);
    grammar_pgsql_select &operator=(const grammar_pgsql_select &);

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_select(*this));
    }
};

#endif
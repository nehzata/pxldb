#ifndef LLM_GRAMMAR_PGSQL_SELECT_CORE_H
#define LLM_GRAMMAR_PGSQL_SELECT_CORE_H

#include <memory>

#include "grammar/grammar.h"

class grammar_pgsql_select_core : public grammar {
   private:
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_pgsql_select_core();
    grammar_pgsql_select_core();

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    grammar_pgsql_select_core(const grammar_pgsql_select_core &);
    grammar_pgsql_select_core &operator=(const grammar_pgsql_select_core &);

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_select_core(*this));
    }
};

#endif
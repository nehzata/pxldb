#ifndef LLM_GRAMMAR_MAX_N_H
#define LLM_GRAMMAR_MAX_N_H

#include <string>

#include "grammar.h"

class grammar_max_n : public grammar {
   private:
    grammar::factory builder;
    std::unique_ptr<grammar> g;
    size_t n_max;
    size_t n_count;
    size_t n_chars;

   public:
    virtual ~grammar_max_n();
    grammar_max_n(grammar::factory builder, size_t n_max);
    grammar_max_n(const grammar_max_n &);
    grammar_max_n &operator=(const grammar_max_n &);

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_max_n(*this));
    }
};

#endif
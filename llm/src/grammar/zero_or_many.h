#ifndef LLM_GRAMMAR_ZERO_OR_MANY_H
#define LLM_GRAMMAR_ZERO_OR_MANY_H

#include <string>

#include "grammar.h"

class grammar_zero_or_many : public grammar {
   private:
    grammar::factory builder;
    std::unique_ptr<grammar> g;
    size_t n_chars;

   public:
    virtual ~grammar_zero_or_many();
    grammar_zero_or_many(grammar::factory builder);
    grammar_zero_or_many(const grammar_zero_or_many &);
    grammar_zero_or_many &operator=(const grammar_zero_or_many &);

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_zero_or_many(*this));
    }
};

#endif
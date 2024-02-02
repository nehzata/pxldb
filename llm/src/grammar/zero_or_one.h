#ifndef LLM_GRAMMAR_ZERO_OR_ONE_H
#define LLM_GRAMMAR_ZERO_OR_ONE_H

#include <string>

#include "grammar.h"

class grammar_zero_or_one : public grammar {
   protected:
    std::unique_ptr<grammar> g;
    size_t n_chars;

   public:
    virtual ~grammar_zero_or_one();
    grammar_zero_or_one(grammar *);
    grammar_zero_or_one(const grammar_zero_or_one &);
    grammar_zero_or_one &operator=(const grammar_zero_or_one &);

    virtual grammar_result eval(unsigned int depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_zero_or_one(*this));
    }
};

#endif
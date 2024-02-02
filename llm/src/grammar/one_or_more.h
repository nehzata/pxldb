#ifndef LLM_GRAMMAR_ONE_OR_MORE_H
#define LLM_GRAMMAR_ONE_OR_MORE_H

#include <string>

#include "list.h"
#include "zero_or_many.h"

class grammar_one_or_more : public grammar_list {
   private:
    grammar::factory builder;

   public:
    virtual ~grammar_one_or_more() {}
    grammar_one_or_more(grammar::factory builder)
        : grammar_list({builder(), new grammar_zero_or_many(builder)}), builder(builder) {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_one_or_more(*this));
    }
};

#endif
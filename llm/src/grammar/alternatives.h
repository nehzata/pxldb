#ifndef LLM_GRAMMAR_ALTERNATIVES_H
#define LLM_GRAMMAR_ALTERNATIVES_H

#include <list>
#include <memory>
#include <vector>

#include "grammar/grammar.h"

class grammar_alternatives : public grammar {
   private:
    std::list<std::unique_ptr<grammar>> list;
    std::list<std::unique_ptr<grammar>>::iterator it;
    uint idx;
    size_t n_char;

   public:
    virtual ~grammar_alternatives();
    grammar_alternatives(const std::initializer_list<grammar *> &opts);
    grammar_alternatives(const grammar_alternatives &);
    grammar_alternatives &operator=(const grammar_alternatives &);

    virtual grammar_result eval(uint depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_alternatives(*this));
    }
};

#endif
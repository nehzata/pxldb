#ifndef LLM_GRAMMAR_LIST_H
#define LLM_GRAMMAR_LIST_H

#include <list>
#include <memory>

#include "grammar.h"

class grammar_list : public grammar {
   protected:
    std::list<std::unique_ptr<grammar>> list;
    std::list<std::unique_ptr<grammar>>::iterator it;
    uint idx;

   public:
    virtual ~grammar_list();
    grammar_list(const std::initializer_list<grammar *> &list);
    grammar_list(const grammar_list &);
    grammar_list &operator=(const grammar_list &);

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_list(*this));
    }
};

#endif
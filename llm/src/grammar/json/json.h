#ifndef LLM_GRAMMAR_JSON_H
#define LLM_GRAMMAR_JSON_H

#include <list>
#include <memory>

#include "grammar/grammar.h"

class grammar_json : public grammar {
   private:
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_json();
    grammar_json();

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    grammar_json(const grammar_json &);
    grammar_json &operator=(const grammar_json &);

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_json(*this));
    }
};

#endif
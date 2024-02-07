#ifndef LLM_GRAMMAR_IDENTIFIER_H
#define LLM_GRAMMAR_IDENTIFIER_H

#include <string>

#include "grammar.h"

class grammar_identifier : public grammar {
   protected:
    std::string identifier;
    std::string::iterator it;

   public:
    virtual ~grammar_identifier();
    grammar_identifier(const char *identifier);
    grammar_identifier(const grammar_identifier &);
    grammar_identifier &operator=(const grammar_identifier &);

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_identifier(*this));
    }
};

#endif
#ifndef LLM_GRAMMAR_IDENTIFIER_CI_H
#define LLM_GRAMMAR_IDENTIFIER_CI_H

#include <string>
#include <vector>

#include "grammar.h"

class grammar_identifier_ci : public grammar {
   protected:
    std::string str;
    std::vector<std::pair<char, char>> identifier;
    std::vector<std::pair<char, char>>::iterator it;

   public:
    virtual ~grammar_identifier_ci();
    grammar_identifier_ci(const char *term);
    grammar_identifier_ci(const grammar_identifier_ci &);
    grammar_identifier_ci &operator=(const grammar_identifier_ci &);

    virtual grammar_result_code eval(uint depth, buffer &b) override;

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_identifier_ci(*this));
    }
};

#endif
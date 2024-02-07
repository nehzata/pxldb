#ifndef LLM_GRAMMAR_PGSQL_IDENTIFIER_H
#define LLM_GRAMMAR_PGSQL_IDENTIFIER_H

#include <spdlog/spdlog.h>

#include "grammar/grammar.h"
#include "keywords.h"

// // clang-format off
// static std::string KEYWORDS[] = {
//     "SELECT", "FROM", "WHERE", "GROUP", "BY"
// };
// // clang-format on

class grammar_pgsql_identifier : public grammar {
   private:
    std::string str;

   public:
    virtual ~grammar_pgsql_identifier() {}
    grammar_pgsql_identifier() {}

    grammar_result_code eval(uint depth, buffer &b) override {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} pgsql_identifier: {},null", std::string(depth, ' '), str);
            for (unsigned long i = 0; i < sizeof(KEYWORDS) / sizeof(std::string); i++) {
                if (KEYWORDS[i] == str) {
                    // return {GRAMMAR_RESULT_ERROR, 0};
                    return GRAMMAR_RESULT_ERROR;
                }
            }
            // return {str.size() ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR, 0};
            return str.size() ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR;
        }

        const char c = v.second;
        spdlog::debug("{} pgsql_identifier: {},{}", std::string(depth, ' '), str, c);
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') {
            str.push_back(c);
            // return {GRAMMAR_RESULT_CONTINUE, 0};
            return GRAMMAR_RESULT_CONTINUE;
        } else if (str.length()) {
            for (unsigned long i = 0; i < sizeof(KEYWORDS) / sizeof(std::string); i++) {
                if (KEYWORDS[i] == str) {
                    // return {GRAMMAR_RESULT_ERROR, 0};
                    return GRAMMAR_RESULT_ERROR;
                }
            }
            // return {GRAMMAR_RESULT_FINISH, 1};
            b.rewind(1);
            return GRAMMAR_RESULT_FINISH;
        }
        // return {GRAMMAR_RESULT_ERROR, 0};
        return GRAMMAR_RESULT_ERROR;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_identifier(*this));
    }
};

#endif
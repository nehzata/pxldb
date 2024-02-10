#ifndef LLM_GRAMMAR_PGSQL_KEYWORD_H
#define LLM_GRAMMAR_PGSQL_KEYWORD_H

#include <spdlog/spdlog.h>

#include "grammar/grammar.h"
#include "keywords.h"

class grammar_pgsql_keyword : public grammar {
   private:
    const std::string *keywords;
    const size_t n_keywords;
    std::string str;

   public:
    virtual ~grammar_pgsql_keyword() {}
    grammar_pgsql_keyword(const std::string *keywords, const size_t n_keywords)
        : keywords(keywords), n_keywords(n_keywords) {}

    grammar_result_code eval(uint depth, buffer &b) override {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} pgsql_keyword: {},null", std::string(depth, ' '), str);
            for (unsigned long i = 0; i < n_keywords; i++) {
                if (keywords[i] == str) {
                    // return {GRAMMAR_RESULT_ERROR, 0};
                    return GRAMMAR_RESULT_FINISH;
                }
            }
            return GRAMMAR_RESULT_ERROR;
            // return {str.size() ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR, 0};
            // return str.size() ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR;
        }

        const char c = v.second;
        spdlog::debug("{} pgsql_keyword: {},{}", std::string(depth, ' '), str, c);
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_') {
            str.push_back(std::toupper(c));
            // return {GRAMMAR_RESULT_CONTINUE, 0};
            return GRAMMAR_RESULT_CONTINUE;
        } else if (str.length()) {
            for (unsigned long i = 0; i < n_keywords; i++) {
                spdlog::debug("checking {}", keywords[i]);
                if (keywords[i] == str) {
                    // return {GRAMMAR_RESULT_FINISH, 1};
                    b.rewind(1);
                    return GRAMMAR_RESULT_FINISH;
                }
            }
        }
        // return {GRAMMAR_RESULT_ERROR, 0};
        return GRAMMAR_RESULT_ERROR;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_keyword(*this));
    }
};

class grammar_pgsql_keyword_reserved : public grammar_pgsql_keyword {
   public:
    virtual ~grammar_pgsql_keyword_reserved() {}
    grammar_pgsql_keyword_reserved()
        : grammar_pgsql_keyword(RESERVED_KEYWORDS, sizeof(RESERVED_KEYWORDS) / sizeof(std::string)) {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_keyword_reserved(*this));
    }
};

class grammar_pgsql_keyword_unreserved : public grammar_pgsql_keyword {
   public:
    virtual ~grammar_pgsql_keyword_unreserved() {}
    grammar_pgsql_keyword_unreserved()
        : grammar_pgsql_keyword(UNRESERVED_KEYWORDS, sizeof(UNRESERVED_KEYWORDS) / sizeof(std::string)) {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_keyword_unreserved(*this));
    }
};

class grammar_pgsql_keyword_col_name : public grammar_pgsql_keyword {
   public:
    virtual ~grammar_pgsql_keyword_col_name() {}
    grammar_pgsql_keyword_col_name()
        : grammar_pgsql_keyword(COL_NAME_KEYWORDS, sizeof(COL_NAME_KEYWORDS) / sizeof(std::string)) {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_keyword_col_name(*this));
    }
};

class grammar_pgsql_keyword_func_name : public grammar_pgsql_keyword {
   public:
    virtual ~grammar_pgsql_keyword_func_name() {}
    grammar_pgsql_keyword_func_name()
        : grammar_pgsql_keyword(FUNC_NAME_KEYWORDS, sizeof(FUNC_NAME_KEYWORDS) / sizeof(std::string)) {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_keyword_func_name(*this));
    }
};

class grammar_pgsql_keyword_bare_label : public grammar_pgsql_keyword {
   public:
    virtual ~grammar_pgsql_keyword_bare_label() {}
    grammar_pgsql_keyword_bare_label()
        : grammar_pgsql_keyword(BARE_LABEL_KEYWORDS, sizeof(BARE_LABEL_KEYWORDS) / sizeof(std::string)) {}

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_keyword_bare_label(*this));
    }
};

#endif
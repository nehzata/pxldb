#ifndef LLM_GRAMMAR_REGEX_H
#define LLM_GRAMMAR_REGEX_H

#include <spdlog/spdlog.h>

#include <regex>
#include <string>

#include "grammar.h"

class grammar_regex : public grammar {
   private:
    std::string str;
    std::regex r;

   public:
    virtual ~grammar_regex() {}
    // clang-format off
    grammar_regex(std::string r) : str(r), r(r) {}
    //clang-format on

    virtual grammar_result_code eval(uint depth, buffer &b) override {
      const std::pair<bool, char> v = b.next();
      if (!v.first) {
        spdlog::debug("{} regex: {},null", std::string(depth, ' '), str);
        // return {GRAMMAR_RESULT_ERROR, 0};
        return GRAMMAR_RESULT_ERROR;
      }

      const char c = v.second;
      spdlog::debug("{} regex: {},{}", std::string(depth, ' '), str, c);

      std::string str({c});
      if (std::regex_search(str, r)) {
        // return {GRAMMAR_RESULT_FINISH, 0};
        return GRAMMAR_RESULT_FINISH;
      }
      // return {GRAMMAR_RESULT_ERROR, 0};
      return GRAMMAR_RESULT_ERROR;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_regex(*this));
    }
};

#endif
#ifndef LLM_GRAMMAR_PGSQL_NUMBER_H
#define LLM_GRAMMAR_PGSQL_NUMBER_H

#include <spdlog/spdlog.h>

#include "grammar/grammar.h"

enum grammar_pgsql_number_state {
    GRAMMAR_PGSQL_NUMBER_STATE_INIT,
    GRAMMAR_PGSQL_NUMBER_STATE_N,
};

class grammar_pgsql_number : public grammar {
   private:
    grammar_pgsql_number_state state;

   public:
    virtual ~grammar_pgsql_number() {}
    grammar_pgsql_number() : state(GRAMMAR_PGSQL_NUMBER_STATE_INIT) {}

    virtual grammar_result eval(uint depth, buffer &b) override {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} pgsql_number: {},null", std::string(depth, ' '), int(state));
            return {state == GRAMMAR_PGSQL_NUMBER_STATE_INIT ? GRAMMAR_RESULT_ERROR : GRAMMAR_RESULT_FINISH, 0};
        }

        const char c = v.second;
        spdlog::debug("{} pgsql_number: {},{}", std::string(depth, ' '), int(state), c);
        switch (state) {
            case GRAMMAR_PGSQL_NUMBER_STATE_INIT:
                if (c >= '0' && c <= '9') {
                    state = GRAMMAR_PGSQL_NUMBER_STATE_N;
                    return {GRAMMAR_RESULT_CONTINUE, 0};
                }
                break;
            case GRAMMAR_PGSQL_NUMBER_STATE_N:
                if (c >= '0' && c <= '9') {
                    state = GRAMMAR_PGSQL_NUMBER_STATE_N;
                    return {GRAMMAR_RESULT_CONTINUE, 0};
                }
                return {GRAMMAR_RESULT_FINISH, 1};
        }
        return {GRAMMAR_RESULT_ERROR, 0};
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_number(*this));
    }
};

#endif
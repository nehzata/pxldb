#ifndef LLM_GRAMMAR_PGSQL_STRING_H
#define LLM_GRAMMAR_PGSQL_STRING_H

#include <spdlog/spdlog.h>

#include "grammar/grammar.h"

enum grammar_pgsql_string_state {
    GRAMMAR_PGSQL_STRING_STATE_INIT,
    GRAMMAR_PGSQL_STRING_STATE_QUOTE_OPEN,
    GRAMMAR_PGSQL_STRING_STATE_C,
    GRAMMAR_PGSQL_STRING_STATE_ESCAPE,
};

class grammar_pgsql_string : public grammar {
   private:
    grammar_pgsql_string_state state;

   public:
    virtual ~grammar_pgsql_string() {}
    grammar_pgsql_string() : state(GRAMMAR_PGSQL_STRING_STATE_INIT) {}
    virtual grammar_result eval(uint depth, buffer &b) override {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} pgsql_string: {},null", std::string(depth, ' '), int(state));
            return {state == GRAMMAR_PGSQL_STRING_STATE_ESCAPE ? GRAMMAR_RESULT_FINISH : GRAMMAR_RESULT_ERROR, 0};
        }

        const char c = v.second;
        spdlog::debug("{} pgsql_string: {},{}", std::string(depth, ' '), int(state), c);
        switch (state) {
            case GRAMMAR_PGSQL_STRING_STATE_INIT:
                if (c == '\'') {
                    state = GRAMMAR_PGSQL_STRING_STATE_QUOTE_OPEN;
                    return {GRAMMAR_RESULT_CONTINUE, 0};
                }
                break;
            case GRAMMAR_PGSQL_STRING_STATE_QUOTE_OPEN:
                if (c == '\'') {
                    state = GRAMMAR_PGSQL_STRING_STATE_ESCAPE;
                    return {GRAMMAR_RESULT_CONTINUE, 0};
                }
                state = GRAMMAR_PGSQL_STRING_STATE_C;
                return {GRAMMAR_RESULT_CONTINUE, 0};
            case GRAMMAR_PGSQL_STRING_STATE_C:
                if (c == '\'') {
                    state = GRAMMAR_PGSQL_STRING_STATE_ESCAPE;
                    return {GRAMMAR_RESULT_CONTINUE, 0};
                }
                return {GRAMMAR_RESULT_CONTINUE, 0};
            case GRAMMAR_PGSQL_STRING_STATE_ESCAPE:
                if (c == '\'') {
                    state = GRAMMAR_PGSQL_STRING_STATE_C;
                    return {GRAMMAR_RESULT_CONTINUE, 0};
                }
                return {GRAMMAR_RESULT_FINISH, 1};
        }
        return {GRAMMAR_RESULT_ERROR, 0};
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_string(*this));
    }
};

#endif
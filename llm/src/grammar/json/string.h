#ifndef LLM_GRAMMAR_JSON_STRING_H
#define LLM_GRAMMAR_JSON_STRING_H

#include "grammar/grammar.h"

enum grammar_json_string_state {
    GRAMMAR_JSON_STRING_STATE_INIT,
    GRAMMAR_JSON_STRING_STATE_QUOTE_OPEN,
    GRAMMAR_JSON_STRING_STATE_C,
    GRAMMAR_JSON_STRING_STATE_ESCAPE,
};

class grammar_json_string : public grammar {
   private:
    grammar_json_string_state state;

   public:
    virtual ~grammar_json_string() {}
    grammar_json_string() : state(GRAMMAR_JSON_STRING_STATE_INIT) {}
    virtual grammar_result_code eval(uint depth, buffer &b) override {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} json_number: {},null", std::string(depth, ' '), int(state));
            // return {GRAMMAR_RESULT_ERROR, 0};
            return GRAMMAR_RESULT_ERROR;
        }

        const char c = v.second;
        spdlog::debug("{} json_string: {},{}", std::string(depth, ' '), int(state), c);
        switch (state) {
            case GRAMMAR_JSON_STRING_STATE_INIT:
                if (c == '"') {
                    state = GRAMMAR_JSON_STRING_STATE_QUOTE_OPEN;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                break;
            case GRAMMAR_JSON_STRING_STATE_QUOTE_OPEN:
                if (c == '"') {
                    // return {GRAMMAR_RESULT_FINISH, 0};
                    return GRAMMAR_RESULT_FINISH;
                } else if (c == '\\') {
                    state = GRAMMAR_JSON_STRING_STATE_ESCAPE;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ') {
                    state = GRAMMAR_JSON_STRING_STATE_C;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                break;
            case GRAMMAR_JSON_STRING_STATE_C:
                if (c == '"') {
                    // return {GRAMMAR_RESULT_FINISH, 0};
                    return GRAMMAR_RESULT_FINISH;
                } else if (c == '\\') {
                    state = GRAMMAR_JSON_STRING_STATE_ESCAPE;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ') {
                    state = GRAMMAR_JSON_STRING_STATE_C;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                break;
            case GRAMMAR_JSON_STRING_STATE_ESCAPE:
                if (c == '\\' || c == '"' || c == 'n' || c == 's' || c == 't') {
                    state = GRAMMAR_JSON_STRING_STATE_C;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                break;
        }
        // return {GRAMMAR_RESULT_ERROR, 0};
        return GRAMMAR_RESULT_ERROR;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_json_string(*this));
    }
};

#endif
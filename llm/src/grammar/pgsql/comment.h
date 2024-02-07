#ifndef LLM_GRAMMAR_PGSQL_COMMENT_H
#define LLM_GRAMMAR_PGSQL_COMMENT_H

#include <spdlog/spdlog.h>

#include "grammar/grammar.h"

enum grammar_pgsql_comment_state {
    GRAMMAR_PGSQL_COMMENT_STATE_IDLE,
    GRAMMAR_PGSQL_COMMENT_STATE_DASH,
    GRAMMAR_PGSQL_COMMENT_STATE_COMMENTING,
    GRAMMAR_PGSQL_COMMENT_STATE_NEW_LINE
};

// TODO
// this is an incomplete way of handling comments in the parser
// eg if a -- appears in a grammar_pgsql_identifier
// it'll change the path of the parser and will be skipped until the next \n
// this comment parser needs a way to know if its child grammar is currently
// inside such an element and ignore --

class grammar_pgsql_comment : public grammar {
   private:
    grammar_pgsql_comment_state state;
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_pgsql_comment() {}
    grammar_pgsql_comment(grammar *g) : state(GRAMMAR_PGSQL_COMMENT_STATE_IDLE), g(std::unique_ptr<grammar>(g)) {}

    virtual grammar_result_code eval(uint depth, buffer &b) override {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} pgsql_comment: {},null", std::string(depth, ' '), int(state));
            if (state == GRAMMAR_PGSQL_COMMENT_STATE_IDLE) {
                return g->eval(depth, b);
            }
            // return {GRAMMAR_RESULT_FINISH, 0};
            return GRAMMAR_RESULT_FINISH;
        }

        const char c = v.second;
        spdlog::debug("{} pgsql_comment: {},{}", std::string(depth, ' '), int(state), c);
        switch (state) {
            case GRAMMAR_PGSQL_COMMENT_STATE_IDLE:
                if (c == '-') {
                    state = GRAMMAR_PGSQL_COMMENT_STATE_DASH;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                b.rewind(1);
                break;
            case GRAMMAR_PGSQL_COMMENT_STATE_DASH:
                if (c == '-') {
                    state = GRAMMAR_PGSQL_COMMENT_STATE_COMMENTING;
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                state = GRAMMAR_PGSQL_COMMENT_STATE_IDLE;
                b.rewind(2);
                break;
            case GRAMMAR_PGSQL_COMMENT_STATE_COMMENTING:
                if (c == '\n') {
                    state = GRAMMAR_PGSQL_COMMENT_STATE_NEW_LINE;
                }
                // return {GRAMMAR_RESULT_CONTINUE, 0};
                return GRAMMAR_RESULT_CONTINUE;
            case GRAMMAR_PGSQL_COMMENT_STATE_NEW_LINE:
                if (c == '\n') {
                    // return {GRAMMAR_RESULT_CONTINUE, 0};
                    return GRAMMAR_RESULT_CONTINUE;
                }
                state = GRAMMAR_PGSQL_COMMENT_STATE_IDLE;
                b.rewind(1);
                break;
        }
        return g->eval(depth, b);
    }

    grammar_pgsql_comment(const grammar_pgsql_comment &src) : state(src.state) {
        if (src.g) {
            g = std::unique_ptr<grammar>(src.g->clone());
        }
    }
    grammar_pgsql_comment &operator=(const grammar_pgsql_comment &src) {
        if (this == &src) {
            return *this;
        }
        if (src.g) {
            g = std::unique_ptr<grammar>(src.g->clone());
        }
        state = src.state;
        return *this;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_comment(*this));
    }
};

#endif
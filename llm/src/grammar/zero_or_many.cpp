#include "zero_or_many.h"

#include <spdlog/spdlog.h>

grammar_zero_or_many::~grammar_zero_or_many() {}

grammar_zero_or_many::grammar_zero_or_many(grammar::factory builder) : builder(builder), n_chars(0) {}

grammar_zero_or_many::grammar_zero_or_many(const grammar_zero_or_many &src)
    : builder(src.builder), n_chars(src.n_chars) {
    if (src.g) {
        g = src.g->clone();
    }
}

grammar_zero_or_many &grammar_zero_or_many::operator=(const grammar_zero_or_many &src) {
    if (this == &src) {
        return *this;
    }

    builder = src.builder;
    if (src.g) {
        g = src.g->clone();
    }
    n_chars = src.n_chars;
    return *this;
}

grammar_result grammar_zero_or_many::eval(uint depth, buffer &b) {
    if (!g) {
        g = std::unique_ptr<grammar>(builder());
    }
    grammar_result r = g->eval(depth + 1, b);
    spdlog::debug("{} zero_or_many: {},{} n_chars:{}", std::string(depth, ' '), int(r.code), r.n_rewind, n_chars);
    switch (r.code) {
        case GRAMMAR_RESULT_ERROR:
            if (!n_chars) {
                return {GRAMMAR_RESULT_FINISH, 1};
            }
            return {GRAMMAR_RESULT_FINISH, n_chars + 1};
        case GRAMMAR_RESULT_CONTINUE:
            n_chars -= r.n_rewind;
            n_chars++;
            return r;
        case GRAMMAR_RESULT_FINISH:
            g = std::unique_ptr<grammar>(builder());
            n_chars = 0;
            return {GRAMMAR_RESULT_CONTINUE, r.n_rewind};
    }
}
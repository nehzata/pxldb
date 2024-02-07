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

grammar_result_code grammar_zero_or_many::eval(uint depth, buffer &b) {
    if (!g) {
        g = std::unique_ptr<grammar>(builder());
    }
    size_t pos_start = b.pos();
    grammar_result_code r = g->eval(depth + 1, b);
    spdlog::debug("{} zero_or_many: {} n_chars:{}", std::string(depth, ' '), int(r), n_chars);
    switch (r) {
        case GRAMMAR_RESULT_ERROR:
            if (!n_chars) {
                // return {GRAMMAR_RESULT_FINISH, 1};
                b.rewind(b.pos() - pos_start);
                return GRAMMAR_RESULT_FINISH;
            }
            // return {GRAMMAR_RESULT_FINISH, n_chars + 1};
            b.rewind(n_chars + b.pos() - pos_start);
            return GRAMMAR_RESULT_FINISH;
        case GRAMMAR_RESULT_CONTINUE:
            // n_chars -= r.n_rewind;
            // n_chars++;
            n_chars += (b.pos() - pos_start);
            return r;
        case GRAMMAR_RESULT_FINISH:
            g = std::unique_ptr<grammar>(builder());
            n_chars = 0;
            // return {GRAMMAR_RESULT_CONTINUE, r.n_rewind};
            return GRAMMAR_RESULT_CONTINUE;
    }
}
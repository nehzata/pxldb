#include "max_n.h"

#include <spdlog/spdlog.h>

grammar_max_n::~grammar_max_n() {}

grammar_max_n::grammar_max_n(grammar::factory builder, size_t n_max)
    : builder(builder), n_max(n_max), n_count(0), n_chars(0) {}

grammar_max_n::grammar_max_n(const grammar_max_n &src)
    : builder(src.builder), n_max(src.n_max), n_count(src.n_count), n_chars(src.n_chars) {
    if (src.g) {
        g = src.g->clone();
    }
}

grammar_max_n &grammar_max_n::operator=(const grammar_max_n &src) {
    if (this == &src) {
        return *this;
    }

    builder = src.builder;
    if (src.g) {
        g = src.g->clone();
    }
    n_max = src.n_max;
    n_count = src.n_count;
    n_chars = src.n_chars;
    return *this;
}

grammar_result_code grammar_max_n::eval(uint depth, buffer &b) {
    if (!g) {
        g = std::unique_ptr<grammar>(builder());
    }
    size_t pos_start = b.pos();
    grammar_result_code r = g->eval(depth + 1, b);
    spdlog::debug("{} max_n({}): {} n_count:{}, n_chars:{}", std::string(depth, ' '), n_max, int(r), n_count, n_chars);
    switch (r) {
        case GRAMMAR_RESULT_ERROR:
            if (!n_count) {
                return r;
            } else if (!n_chars) {
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
            if (++n_count >= n_max) {
                return r;
            }
            g = std::unique_ptr<grammar>(builder());
            n_chars = 0;
            // return {GRAMMAR_RESULT_CONTINUE, r.n_rewind};
            return GRAMMAR_RESULT_CONTINUE;
    }
}
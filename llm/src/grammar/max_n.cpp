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

grammar_result grammar_max_n::eval(uint depth, buffer &b) {
    if (!g) {
        g = std::unique_ptr<grammar>(builder());
    }
    grammar_result r = g->eval(depth + 1, b);
    spdlog::debug("{} max_n({}): {},{} n_count:{}, n_chars:{}", std::string(depth, ' '), n_max, int(r.code), r.n_rewind,
                  n_count, n_chars);
    switch (r.code) {
        case GRAMMAR_RESULT_ERROR:
            if (!n_count) {
                return r;
            } else if (!n_chars) {
                return {GRAMMAR_RESULT_FINISH, 1};
            }
            return {GRAMMAR_RESULT_FINISH, n_chars + 1};
        case GRAMMAR_RESULT_CONTINUE:
            n_chars -= r.n_rewind;
            n_chars++;
            return r;
        case GRAMMAR_RESULT_FINISH:
            if (++n_count >= n_max) {
                return r;
            }
            g = std::unique_ptr<grammar>(builder());
            n_chars = 0;
            return {GRAMMAR_RESULT_CONTINUE, r.n_rewind};
    }
}
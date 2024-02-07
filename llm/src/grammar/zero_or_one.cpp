#include "zero_or_one.h"

#include <spdlog/spdlog.h>

grammar_zero_or_one::~grammar_zero_or_one() {}

grammar_zero_or_one::grammar_zero_or_one(grammar *g) : g(std::unique_ptr<grammar>(g)), n_chars(0) {}

grammar_zero_or_one::grammar_zero_or_one(const grammar_zero_or_one &src) : n_chars(src.n_chars) { g = src.g->clone(); }

grammar_zero_or_one &grammar_zero_or_one::operator=(const grammar_zero_or_one &src) {
    if (this == &src) {
        return *this;
    }

    g = src.g->clone();
    n_chars = src.n_chars;

    return *this;
}

grammar_result_code grammar_zero_or_one::eval(uint depth, buffer &b) {
    size_t pos_start = b.pos();
    grammar_result_code r = g->eval(depth + 1, b);
    spdlog::debug("{} zero_or_one: {} n_chars:{}", std::string(depth, ' '), int(r), n_chars);
    switch (r) {
        case GRAMMAR_RESULT_ERROR:
            // return {GRAMMAR_RESULT_FINISH, n_chars + r.n_rewind};
            b.rewind(n_chars + b.pos() - pos_start);
            return GRAMMAR_RESULT_FINISH;
        case GRAMMAR_RESULT_CONTINUE:
        case GRAMMAR_RESULT_FINISH:
            // n_chars -= r.n_rewind;
            // n_chars++;
            n_chars += (b.pos() - pos_start);
            return r;
    }
}
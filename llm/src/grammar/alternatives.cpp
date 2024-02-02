#include "grammar/alternatives.h"

#include <spdlog/spdlog.h>

grammar_alternatives::~grammar_alternatives() { list.clear(); }

grammar_alternatives::grammar_alternatives(const std::initializer_list<grammar *> &opts) : idx(0), n_char(0) {
    for (std::initializer_list<grammar *>::iterator i = opts.begin(); i != opts.end(); i++) {
        list.push_back(std::unique_ptr<grammar>(*i));
    }
    it = list.begin();
}

grammar_alternatives::grammar_alternatives(const grammar_alternatives &src) : idx(src.idx), n_char(src.n_char) {
    for (std::list<std::unique_ptr<grammar>>::const_iterator _it = src.list.begin(); _it != src.list.end(); ++_it) {
        grammar *r = _it->get();
        list.push_back(r->clone());
        if (_it == src.it) {
            it = std::next(list.end(), -1);
        }
    }
}

grammar_alternatives &grammar_alternatives::operator=(const grammar_alternatives &src) {
    if (this == &src) {
        return *this;
    }

    list.clear();
    for (std::list<std::unique_ptr<grammar>>::const_iterator _it = src.list.begin(); _it != src.list.end(); ++_it) {
        grammar *r = _it->get();
        list.push_back(r->clone());
        if (_it == src.it) {
            it = std::next(list.end(), -1);
        }
    }
    idx = src.idx;
    n_char = src.n_char;

    return *this;
}

grammar_result grammar_alternatives::eval(uint depth, buffer &b) {
    grammar_result r = it->get()->eval(depth + 1, b);
    spdlog::debug("{} alternatives: {},{} idx:{}, n_chars:{}", std::string(depth, ' '), int(r.code), r.n_rewind, idx,
                  n_char);
    switch (r.code) {
        case GRAMMAR_RESULT_ERROR: {
            idx++;
            if (++it == list.end()) {
                return r;
            }

            grammar_result res = {GRAMMAR_RESULT_CONTINUE, n_char + 1};
            n_char = 0;

            return res;
        }
        case GRAMMAR_RESULT_CONTINUE:
        case GRAMMAR_RESULT_FINISH:
            n_char -= r.n_rewind;
            n_char++;
            return r;
    }
}
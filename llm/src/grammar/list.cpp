#include "list.h"

#include <spdlog/spdlog.h>

grammar_list::~grammar_list() { list.clear(); }

grammar_list::grammar_list(const std::initializer_list<grammar *> &items) : idx(0) {
    for (std::initializer_list<grammar *>::iterator i = items.begin(); i != items.end(); i++) {
        list.push_back(std::unique_ptr<grammar>(*i));
    }
    it = list.begin();
}

grammar_list::grammar_list(const grammar_list &src) : idx(src.idx) {
    for (std::list<std::unique_ptr<grammar>>::const_iterator _it = src.list.begin(); _it != src.list.end(); ++_it) {
        grammar *r = _it->get();
        list.push_back(r->clone());
        if (_it == src.it) {
            it = std::next(list.end(), -1);
        }
    }
}

grammar_list &grammar_list::operator=(const grammar_list &src) {
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

    return *this;
}

grammar_result_code grammar_list::eval(uint depth, buffer &b) {
    grammar_result_code r = it->get()->eval(depth + 1, b);
    spdlog::debug("{} list: {} idx:{}", std::string(depth, ' '), int(r), idx);
    switch (r) {
        case GRAMMAR_RESULT_ERROR:
        case GRAMMAR_RESULT_CONTINUE:
            return r;
        case GRAMMAR_RESULT_FINISH:
            idx++;
            if (++it != list.end()) {
                // return {GRAMMAR_RESULT_CONTINUE, r.n_rewind};
                return GRAMMAR_RESULT_CONTINUE;
            }
            return r;
    }
}
#include "identifier_ci.h"

#include <spdlog/spdlog.h>

grammar_identifier_ci::~grammar_identifier_ci() {}

grammar_identifier_ci::grammar_identifier_ci(const char *term) : str(term) {
    for (auto &tmp : str) {
        identifier.push_back(std::pair<char, char>(toupper(tmp), tolower(tmp)));
    }
    it = this->identifier.begin();
}

grammar_identifier_ci::grammar_identifier_ci(const grammar_identifier_ci &src)
    : str(src.str), identifier(src.identifier) {
    it = identifier.begin();
    for (auto _it = src.identifier.begin(); _it != src.it; _it++, it++) {
    }
}

grammar_identifier_ci &grammar_identifier_ci::operator=(const grammar_identifier_ci &src) {
    if (this == &src) {
        return *this;
    }

    str = src.str;
    identifier = src.identifier;
    it = identifier.begin();
    for (auto _it = src.identifier.begin(); _it != src.it; _it++, it++) {
    }

    return *this;
}

grammar_result grammar_identifier_ci::eval(uint depth, buffer &b) {
    if (identifier.size() && it != identifier.end()) {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} identifier_ci: {},null", std::string(depth, ' '), str);
            return {it == identifier.begin() ? GRAMMAR_RESULT_ERROR : GRAMMAR_RESULT_FINISH, 0};
        }

        const char c = v.second;
        spdlog::debug("{} identifier_ci: {}:{},{}", std::string(depth, ' '), str, it->first, c);
        if (c == it->first || c == it->second) {
            if (++it == identifier.end()) {
                return {GRAMMAR_RESULT_FINISH, 0};
            }
            return {GRAMMAR_RESULT_CONTINUE, 0};
        }
        return {GRAMMAR_RESULT_ERROR, 0};
    }
    return {GRAMMAR_RESULT_ERROR, 0};
}
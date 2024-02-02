#include "identifier.h"

#include <spdlog/spdlog.h>

grammar_identifier::~grammar_identifier() {}

grammar_identifier::grammar_identifier(const char *identifier) : identifier(identifier) {
    it = this->identifier.begin();
}

grammar_identifier::grammar_identifier(const grammar_identifier &src) {
    identifier = src.identifier;
    it = identifier.begin();
    for (std::string::const_iterator _it = src.identifier.begin(); _it != src.it; _it++, it++) {
    }
}

grammar_identifier &grammar_identifier::operator=(const grammar_identifier &src) {
    if (this == &src) {
        return *this;
    }

    identifier = src.identifier;
    it = identifier.begin();
    for (std::string::const_iterator _it = src.identifier.begin(); _it != src.it; _it++, it++) {
    }

    return *this;
}

grammar_result grammar_identifier::eval(uint depth, buffer &b) {
    if (identifier.length() && it != identifier.end()) {
        const std::pair<bool, char> v = b.next();
        if (!v.first) {
            spdlog::debug("{} identifier: {},null", std::string(depth, ' '), identifier);
            return {it == identifier.begin() ? GRAMMAR_RESULT_ERROR : GRAMMAR_RESULT_FINISH, 0};
        }

        const char c = v.second;
        spdlog::debug("{} identifier: {}:{},{}", std::string(depth, ' '), identifier, *it, c);
        if (c == *it) {
            if (++it == identifier.end()) {
                return {GRAMMAR_RESULT_FINISH, 0};
            }
            return {GRAMMAR_RESULT_CONTINUE, 0};
        }
        return {GRAMMAR_RESULT_ERROR, 0};
    }
    return {GRAMMAR_RESULT_ERROR, 0};
}
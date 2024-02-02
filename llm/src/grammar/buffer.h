#ifndef LLM_BUFFER_H
#define LLM_BUFFER_H

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <vector>

class buffer {
   private:
    std::vector<char> s;
    size_t n;

   public:
    buffer() : n(0) {}

    buffer(const buffer &src) : s(src.s), n(src.n) {}

    buffer &operator=(const buffer &src) {
        if (this == &src) {
            return *this;
        }
        // printf("buffer assign\n");
        s = src.s;
        n = src.n;
        return *this;
    }

    bool empty() { return s.size() - n == 0; }

    void add(const char c) {
        // printf("add(%c)\n", c);
        s.push_back(c);
    }
    std::pair<bool, char> next() {
        if (n >= s.size()) {
            return std::pair<bool, char>(false, '\0');
        }
        const char c = s[n++];
        // printf("next = %c\n", c);
        return std::pair<bool, char>(true, c);
    }
    void rewind(size_t _n) {
        // print();
        // printf("rewind(%lu)\n", _n);
        n -= _n;
        // print();
    }

    void print() {
        std::string tmp;
        for (size_t i = n; i < s.size(); i++) {
            tmp += fmt::format("{:c}", s[i]);
        }
        spdlog::debug("buffer:<{}>", tmp);
    }
};

#endif
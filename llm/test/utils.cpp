#include "utils.h"

bool run(grammar &g, std::string &in) {
    buffer b;
    for (auto &c : in) {
        b.add(c);
    }

    // drain the remainder of the grammar state
    while (true) {
        b.print();
        grammar_result_code r = g.eval(0, b);
        if (r == GRAMMAR_RESULT_FINISH) {
            if (b.empty()) {
                return true;
            }
            spdlog::error("buffer not empty");
            b.print();
            return false;
        } else if (r != GRAMMAR_RESULT_CONTINUE) {
            return false;
        }
    }
}
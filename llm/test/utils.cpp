#include "utils.h"

bool run(grammar &g, std::string &in) {
    buffer b;
    for (auto &c : in) {
        b.add(c);
        // while (!b.empty()) {
        //     b.print();
        //     grammar_result_code r = g.eval(0, b);
        //     // if (r.n_rewind != 0) {
        //     //     b.rewind(r.n_rewind);
        //     // }
        //     if (r != GRAMMAR_RESULT_CONTINUE) {
        //         return r == GRAMMAR_RESULT_FINISH;
        //     }
        // }
    }

    // drain the remainder of the grammar state
    while (true) {
        grammar_result_code r = g.eval(0, b);
        // if (r.n_rewind != 0) {
        //     b.rewind(r.n_rewind);
        // }
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
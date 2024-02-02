#include "utils.h"

bool run(grammar &g, std::string &in) {
    buffer b;
    for (auto &c : in) {
        b.add(c);
        while (!b.empty()) {
            b.print();
            grammar_result r = g.eval(0, b);
            if (r.n_rewind != 0) {
                b.rewind(r.n_rewind);
            }
            if (r.code != GRAMMAR_RESULT_CONTINUE) {
                return r.code == GRAMMAR_RESULT_FINISH;
            }
        }
    }

    // drain the remainder of the grammar state
    for (int i = 0; i < 100; i++) {
        grammar_result r = g.eval(0, b);
        if (r.n_rewind != 0) {
            b.rewind(r.n_rewind);
        }
        if (r.code == GRAMMAR_RESULT_FINISH) {
            return true;
        } else if (r.code == GRAMMAR_RESULT_ERROR) {
            return false;
        }
    }
    return false;
}
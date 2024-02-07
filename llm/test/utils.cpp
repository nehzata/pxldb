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
    while (true) {
        grammar_result r = g.eval(0, b);
        if (r.n_rewind != 0) {
            b.rewind(r.n_rewind);
        }
        if (r.code == GRAMMAR_RESULT_FINISH) {
            return b.empty();
        } else if (r.code != GRAMMAR_RESULT_CONTINUE) {
            return false;
        }
    }

    return false;
}
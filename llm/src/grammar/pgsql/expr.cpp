#include "expr.h"

#include <regex>

#include "column_name.h"
#include "function.h"
#include "grammar/alternatives.h"
#include "grammar/epsilon.h"
#include "grammar/identifier.h"
#include "grammar/identifier_ci.h"
#include "grammar/list.h"
#include "grammar/one_or_more.h"
#include "grammar/regex.h"
#include "grammar/ws.h"
#include "grammar/zero_or_many.h"
#include "grammar/zero_or_one.h"
#include "identifier.h"
#include "keyword.h"
#include "literal.h"
#include "select.h"
#include "typename.h"

// https://iitd.github.io/col728/lec/parsing.html#:~:text=Left%20Recursion%20%3A%20the%20main%20problem%20with%20Recursive%20Descent%20Parsing
// https://www.reddit.com/r/ProgrammingLanguages/comments/w27u4w/comment/igscu61/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
class grammar_pgsql_expr_dash : public grammar {
   private:
    std::unique_ptr<grammar> g;

   public:
    virtual ~grammar_pgsql_expr_dash() {}
    grammar_pgsql_expr_dash() {}

    virtual grammar_result_code eval(uint depth, buffer &b) override {
        if (!g) {
            // clang-format off
            g = std::unique_ptr<grammar>(
                new grammar_alternatives({
                    new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=collation%2Dname-,expr,NOT,-LIKE
                        new grammar_zero_or_one(       // a1
                            new grammar_list({
                                new grammar_ws(),
                                new grammar_identifier_ci("NOT")
                            })
                        ),
                        new grammar_ws(),             // a1
                        new grammar_alternatives({    // a1
                            new grammar_list({
                                new grammar_alternatives({
                                    new grammar_identifier_ci("LIKE"),
                                    new grammar_identifier_ci("ILIKE"),
                                }),
                                new grammar_ws(),
                                new grammar_pgsql_expr(),
                                new grammar_zero_or_one(
                                    new grammar_list({
                                        new grammar_ws(),
                                        new grammar_identifier_ci("ESCAPE"),
                                        new grammar_ws(),
                                        new grammar_pgsql_expr()
                                    })
                                )
                            }),
                            new grammar_list({
                                new grammar_alternatives({
                                    new grammar_identifier_ci("GLOB"),
                                    new grammar_identifier_ci("REGEXP"),
                                    new grammar_identifier_ci("MATCH")
                                }),
                                new grammar_ws(),
                                new grammar_pgsql_expr()
                            })
                        }),
                        new grammar_pgsql_expr_dash(),      // S'
                    }),
                    new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,expr,expr,-function%2Dname
                        new grammar_zero_or_one(new grammar_ws()),              // a2
                        new grammar_alternatives({                              // a2
                            new grammar_identifier("||"),
                            new grammar_identifier("&&"),
                            new grammar_identifier("!="),
                            new grammar_identifier("<>"),
                            new grammar_identifier("<="),
                            new grammar_identifier(">="),
                            new grammar_regex(R"([=*/%&|^~><+-])"),
                        }),
                        new grammar_zero_or_one(new grammar_ws()),              // a2
                        new grammar_pgsql_expr(),                               // a2
                        new grammar_pgsql_expr_dash(),                          // S'
                    }),
                    new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,expr,expr,-function%2Dname
                        new grammar_ws(),                                       // a2
                        new grammar_identifier_ci("OR"),                           // a2
                        new grammar_ws(),                                       // a2
                        new grammar_pgsql_expr(),                               // a2
                        new grammar_pgsql_expr_dash(),                          // S'
                    }),
                    new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,expr,expr,-function%2Dname
                        new grammar_ws(),                                       // a2
                        new grammar_identifier_ci("AND"),                       // a2
                        new grammar_ws(),                                       // a2
                        new grammar_pgsql_expr(),                               // a2
                        new grammar_pgsql_expr_dash(),                          // S'
                    }),
                    new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,ISNULL,-NOTNULL
                        new grammar_list({                                      // a3
                            new grammar_ws(),
                            new grammar_alternatives({
                                new grammar_identifier_ci("IS NULL"),
                                new grammar_identifier_ci("IS NOT NULL"),
                                new grammar_identifier_ci("NOT NULL")
                            }),
                        }),
                        new grammar_pgsql_expr_dash(),                          // S'
                    }),
                    new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=IN-,(,),-expr
                        new grammar_zero_or_one(                                // a3
                            new grammar_list({
                                new grammar_ws(),
                                new grammar_identifier_ci("NOT")
                            })
                        ),
                        new grammar_list({
                            new grammar_ws(),
                            new grammar_identifier_ci("IN")
                        }),
                        new grammar_ws(),
                        new grammar_alternatives({                              // a3
                            new grammar_list({
                                new grammar_identifier("("),
                                new grammar_pgsql_select(),
                                new grammar_identifier(")"),
                            }),
                            new grammar_list({
                                new grammar_pgsql_expr(),
                                new grammar_zero_or_many([]() {
                                    return new grammar_list({
                                        new grammar_identifier(","),
                                        new grammar_zero_or_one(new grammar_ws()),
                                        new grammar_pgsql_expr()
                                    });
                                })
                            })
                        }),
                        new grammar_pgsql_expr_dash(),                          // S'
                    }),
                    new grammar_list({ // :: casting
                        new grammar_identifier("::"),                           // a4
                        new grammar_pgsql_typename(),                           // a4
                        new grammar_zero_or_one(
                            new grammar_list({
                                new grammar_identifier("("),
                                new grammar_pgsql_expr(),
                                new grammar_zero_or_many([]() {
                                    return new grammar_list({
                                        new grammar_identifier(","),
                                        new grammar_zero_or_one(new grammar_ws()),
                                        new grammar_pgsql_expr()
                                    });
                                }),
                                new grammar_identifier(")"),
                            })
                        ),
                        new grammar_pgsql_expr_dash(),                          // S'
                    }),
                    new grammar_epsilon()
                })
            );
            // clang-format on
        }
        return g->eval(depth + 1, b);
    }

    grammar_pgsql_expr_dash(const grammar_pgsql_expr_dash &src) {
        if (src.g) {
            g = std::unique_ptr<grammar>(src.g->clone());
        }
    }
    grammar_pgsql_expr_dash &operator=(const grammar_pgsql_expr_dash &src) {
        if (this == &src) {
            return *this;
        }
        grammar::operator=(src);
        if (src.g) {
            g = std::unique_ptr<grammar>(src.g->clone());
        }
        return *this;
    }

    virtual std::unique_ptr<grammar> clone() const override {
        return std::unique_ptr<grammar>(new grammar_pgsql_expr_dash(*this));
    }
};

grammar_pgsql_expr::~grammar_pgsql_expr() {}

grammar_pgsql_expr::grammar_pgsql_expr() {}

grammar_result_code grammar_pgsql_expr::eval(uint depth, buffer &b) {
    if (!g) {
        // clang-format off
        g = std::unique_ptr<grammar>(
            new grammar_alternatives({
                new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=CASE,END
                    new grammar_identifier_ci("CASE"),
                    new grammar_zero_or_one(
                        new grammar_list({
                            new grammar_ws(),
                            new grammar_pgsql_expr()
                        })
                    ),
                    new grammar_one_or_more([]() {
                        return new grammar_list({
                            new grammar_ws(),
                            new grammar_identifier_ci("WHEN"),
                            new grammar_ws(),
                            new grammar_pgsql_expr(),
                            new grammar_ws(),
                            new grammar_identifier_ci("THEN"),
                            new grammar_ws(),
                            new grammar_pgsql_expr(),
                        });
                    }),
                    new grammar_zero_or_one(
                        new grammar_list({
                            new grammar_ws(),
                            new grammar_identifier_ci("ELSE"),
                            new grammar_ws(),
                            new grammar_pgsql_expr(),
                        })
                    ),
                    new grammar_ws(),
                    new grammar_identifier_ci("END"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=CAST,)
                    new grammar_identifier_ci("CAST"),
                    new grammar_identifier("("),
                    new grammar_pgsql_expr(),
                    new grammar_ws(),
                    new grammar_identifier_ci("AS"),
                    new grammar_ws(),
                    new grammar_pgsql_typename(),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({
                    new grammar_identifier_ci("EXTRACT"),
                    new grammar_identifier("("),
                    new grammar_pgsql_identifier(),
                    new grammar_ws(),
                    new grammar_identifier_ci("FROM"),
                    new grammar_ws(),
                    new grammar_pgsql_expr(),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({
                    new grammar_identifier_ci("NORMALIZE"),
                    new grammar_identifier("("),
                    new grammar_pgsql_expr(),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({
                    new grammar_identifier_ci("SUBSTRING"),
                    new grammar_identifier("("),
                    new grammar_pgsql_string(),
                    new grammar_identifier(","),
                    new grammar_pgsql_number(),
                    new grammar_identifier(","),
                    new grammar_pgsql_number(),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({
                    new grammar_alternatives({
                        new grammar_identifier_ci("COALESCE"),
                        new grammar_identifier_ci("GREATEST"),
                        new grammar_identifier_ci("LEAST"),
                    }),
                    new grammar_identifier("("),
                    new grammar_pgsql_expr(),
                    new grammar_zero_or_many([]() {
                        return new grammar_list({
                            new grammar_identifier(","),
                            new grammar_zero_or_one(new grammar_ws()),
                            new grammar_pgsql_expr()
                        });
                    }),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=%2C-,NOT,),-CASE
                    new grammar_zero_or_one(
                        new grammar_list({
                            new grammar_zero_or_one(
                                new grammar_list({
                                    new grammar_identifier_ci("NOT"),
                                    new grammar_ws(),
                                })
                            ),
                            new grammar_identifier_ci("EXISTS"),
                            new grammar_ws(),
                        })
                    ),
                    new grammar_identifier("("),
                    new grammar_pgsql_select(),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                // new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=)-,expr,collation%2Dname,-expr
                // }),
                new grammar_list({ // b5 // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,function%2Dname,),-filter%2Dclause
                    new grammar_pgsql_function_name(),
                    new grammar_identifier("("),
                    new grammar_pgsql_function_arguments(),
                    new grammar_identifier(")"),
                    // new grammar_zero_or_one(
                    //     new grammar_list({
                    //         new grammar_ws(),
                    //         new grammar_pgsql_filter_clause()
                    //     })
                    // ),
                    // new grammar_zero_or_one(
                    //     new grammar_list({
                    //         new grammar_ws(),
                    //         new grammar_pgsql_over_clause()
                    //     })
                    // ),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({ // b2 // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=over%2Dclause-,(,),-%2C
                    new grammar_identifier("("),
                    new grammar_pgsql_expr(),
                    new grammar_zero_or_many([] {
                        return new grammar_list({
                            new grammar_identifier(","),
                            new grammar_zero_or_one(new grammar_ws()),
                            new grammar_pgsql_expr()
                        });
                    }),
                    new grammar_identifier(")"),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=unary%2Doperator,expr
                    new grammar_regex(R"([~+-])"),              // b3
                    new grammar_pgsql_expr(),                   // b3
                    new grammar_pgsql_expr_dash()
                }),
                // new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=function%2Dname,function%2Darguments
                // }),
                new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=bind%2Dparameter-,schema%2Dname,column%2Dname,-unary%2Doperator
                    new grammar_alternatives({
                        new grammar_list({
                            new grammar_pgsql_table_name(),
                            new grammar_identifier("."),
                            new grammar_pgsql_column_label(),
                        }),
                        new grammar_pgsql_column_id(),
                    }),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({
                    new grammar_alternatives({ //b1 // https://github.com/postgres/postgres/blob/b83033c3cff556d520281aaec399e47b4f11edbe/src/backend/parser/gram.y#L15396
                        new grammar_identifier_ci("CURRENT_DATE"),
                        new grammar_identifier_ci("CURRENT_TIME"),
                        // CURRENT_TIME()
                        new grammar_identifier_ci("CURRENT_TIMESTAMP"),
                        // CURRENT_TIMESTAMP()
                        new grammar_identifier_ci("LOCALTIME"),
                        // LOCALTIME()
                        new grammar_identifier_ci("LOCALTIMESTAMP"),
                        // LOCALTIMESTAMP()
                        new grammar_identifier_ci("CURRENT_ROLE"),
                        new grammar_identifier_ci("CURRENT_USER"),
                        new grammar_identifier_ci("SESSION_USER"),
                        new grammar_identifier_ci("SYSTEM_USER"),
                        new grammar_identifier_ci("USER"),
                        new grammar_identifier_ci("CURRENT_CATALOG"),
                        new grammar_identifier_ci("CURRENT_SCHEMA"),
                    }),
                    new grammar_pgsql_expr_dash()
                }),
                new grammar_list({ // b6 // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr%3A-,literal%2Dvalue,-bind%2Dparameter
                    new grammar_pgsql_literal(),
                    new grammar_pgsql_expr_dash()
                })
            })
        );
        // clang-format on
    }
    grammar_result_code r = g->eval(depth + 1, b);
    spdlog::debug("{} pgsql_expr: {}", std::string(depth, ' '), int(r));
    return r;
    // return g->eval(depth + 1, b);
}

grammar_pgsql_expr::grammar_pgsql_expr(const grammar_pgsql_expr &src) : grammar(src) {
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
}

grammar_pgsql_expr &grammar_pgsql_expr::operator=(const grammar_pgsql_expr &src) {
    if (this == &src) {
        return *this;
    }
    grammar::operator=(src);
    if (src.g) {
        g = std::unique_ptr<grammar>(src.g->clone());
    }
    return *this;
}

// clang-format off
// new grammar_alternatives({
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=collation%2Dname-,expr,NOT,-LIKE
//         new grammar_pgsql_expr(),      // S
//         new grammar_zero_or_one(       // a1
//             new grammar_list({
//                 new grammar_ws(),
//                 new grammar_identifier("NOT")
//             })
//         ),
//         new grammar_ws(),             // a1
//         new grammar_alternatives({    // a1
//             new grammar_list({
//                 new grammar_alternatives({
//                     new grammar_identifier("LIKE"),
//                     new grammar_identifier("ILIKE"),
//                 }),
//                 new grammar_ws(),
//                 new grammar_pgsql_expr(),
//                 new grammar_zero_or_one(
//                     new grammar_list({
//                         new grammar_ws(),
//                         new grammar_identifier("ESCAPE"),
//                         new grammar_ws(),
//                         new grammar_pgsql_expr()
//                     })
//                 )
//             }),
//             new grammar_list({
//                 new grammar_alternatives({
//                     new grammar_identifier("GLOB"),
//                     new grammar_identifier("REGEXP"),
//                     new grammar_identifier("MATCH")
//                 }),
//                 new grammar_ws(),
//                 new grammar_pgsql_expr()
//             })
//         })
//     }),
//     // new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=)-,expr,collation%2Dname,-expr
//     // }),
//     new grammar_alternatives({ //b1 // https://github.com/postgres/postgres/blob/b83033c3cff556d520281aaec399e47b4f11edbe/src/backend/parser/gram.y#L15396
//     }),
//     new grammar_list({ // b2 // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=over%2Dclause-,(,),-%2C
//         new grammar_identifier("("),
//         new grammar_pgsql_expr(),
//         new grammar_zero_or_many([] {
//             return new grammar_list({
//                 new grammar_identifier(","),
//                 new grammar_zero_or_one(new grammar_ws()),
//                 new grammar_pgsql_expr()
//             });
//         }),
//         new grammar_identifier(")"),
//     }),
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,expr,expr,-function%2Dname
//         new grammar_pgsql_expr(),                                // S
//         new grammar_zero_or_one(new grammar_ws()),               // a2
//         new grammar_alternatives({                               // a2
//             new grammar_identifier("%"),
//             new grammar_identifier("*"),
//             new grammar_identifier("+"),
//             new grammar_identifier("-"),
//             new grammar_identifier("/"),
//             new grammar_identifier("|"),
//             new grammar_identifier("&"),
//             new grammar_identifier("^"),
//             new grammar_identifier("~"),
//             new grammar_identifier("||"),
//             new grammar_identifier("&&"),
//             new grammar_identifier("="),
//             new grammar_identifier("!="),
//             new grammar_identifier("<"),
//             new grammar_identifier(">"),
//             new grammar_identifier("<="),
//             new grammar_identifier(">="),
//             new grammar_identifier("<>"),
//         }),
//         new grammar_zero_or_one(new grammar_ws()),               // a2
//         new grammar_pgsql_expr()                                 // a2
//     }),
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,expr,expr,-function%2Dname
//         new grammar_pgsql_expr(),                                // S
//         new new grammar_ws(),                                    // a2
//         new new grammar_identifier("OR"),                        // a2
//         new new grammar_ws(),                                    // a2
//         new grammar_pgsql_expr()                                 // a2
//     }),
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr-,expr,expr,-function%2Dname
//         new grammar_pgsql_expr(),                                // S
//         new new grammar_ws(),                                    // a2
//         new new grammar_identifier("AND"),                       // a2
//         new new grammar_ws(),                                    // a2
//         new grammar_pgsql_expr()                                 // a2
//     }),
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=unary%2Doperator,expr
//         new grammar_regex(R"([~+-])"),              // b3
//         new grammar_pgsql_expr()                    // b3
//     }),
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=bind%2Dparameter-,schema%2Dname,column%2Dname,-unary%2Doperator
//         new grammar_zero_or_one(                   // b4
//             new grammar_list({
//                 new grammar_pgsql_identifier(),
//                 new grammar_identifier(".")
//             })
//         ),
//         new grammar_pgsql_identifier()
//     }),
//     new grammar_list({ // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=function%2Dname,function%2Darguments
//         new grammar_pgsql_keyword(),              // b5
//         new grammar_identifier("("),
//         new grammar_pgsql_function_arguments(),
//         new grammar_identifier(")"),
//         // new grammar_zero_or_one(
//         //     new grammar_list({
//         //         new grammar_ws(),
//         //         new grammar_pgsql_filter_clause()
//         //     })
//         // ),
//         // new grammar_zero_or_one(
//         //     new grammar_list({
//         //         new grammar_ws(),
//         //         new grammar_pgsql_over_clause()
//         //     })
//         // ),
//      }),
//     new grammar_pgsql_literal(), // b6 // https://www.sqlite.org/syntaxdiagrams.html#expr:~:text=expr%3A-,literal%2Dvalue,-bind%2Dparameter
// })
// clang-format on
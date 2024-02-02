#include <spdlog/spdlog.h>

#include <catch2/catch_all.hpp>

#include "grammar/pgsql/expr.h"
#include "utils.h"

TEST_CASE("pgsql_expr") {
    SECTION("expr basic") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_expr g;
        std::string in = "r.name = 'thanksd'";
        REQUIRE(run(g, in) == true);
    }
    SECTION("expr composite") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_expr g;
        std::string in = "r.name = 'thanksd' AND d.tmp = 'GH'";
        REQUIRE(run(g, in) == true);
    }
    spdlog::set_level(spdlog::level::off);
}
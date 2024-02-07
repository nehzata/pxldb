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
    SECTION("expr basic casting") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_expr g;
        std::string in = "123::INTEGER";
        REQUIRE(run(g, in) == true);
    }
    SECTION("expr compound casting") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_expr g;
        std::string in = "123::NUMERIC(5, 2)";
        REQUIRE(run(g, in) == true);
    }
    SECTION("expr cast as") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_expr g;
        std::string in = "CAST('123' AS INTEGER)";
        REQUIRE(run(g, in) == true);
    }
    // SECTION("expr cast type(expr)") {
    //     // spdlog::set_level(spdlog::level::debug);
    //     grammar_pgsql_expr g;
    //     std::string in = "INTEGER('123')";
    //     REQUIRE(run(g, in) == true);
    // }

    SECTION("error") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_expr g;
        std::string in = "tmp = (SELECT MAX(";
        REQUIRE(run(g, in) != true);
    }
    spdlog::set_level(spdlog::level::off);
}
#include <spdlog/spdlog.h>

#include <catch2/catch_all.hpp>

#include "grammar/pgsql/string.h"
#include "utils.h"

TEST_CASE("pgsql_string") {
    SECTION("empty") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_string g;
        std::string in = "''";
        REQUIRE(run(g, in) == true);
    }
    SECTION("happy path") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_string g;
        std::string in = "'this is a happy string 123 #$% ;)(~)'";
        REQUIRE(run(g, in) == true);
    }
    SECTION("escape path") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql_string g;
        std::string in = "' it''s a little funny'";
        REQUIRE(run(g, in) == true);
    }
    spdlog::set_level(spdlog::level::off);
}
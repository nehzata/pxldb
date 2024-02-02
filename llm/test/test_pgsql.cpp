#include <spdlog/spdlog.h>

#include <catch2/catch_all.hpp>

#include "grammar/pgsql/pgsql.h"
#include "utils.h"

TEST_CASE("pgsql") {
    SECTION("SELECT TRUE") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT TRUE;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("SELECT TRUE FROM test") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT TRUE FROM test;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("SELECT * FROM my_table") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT * FROM my_table;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("SELECT * FROM users WHERE email LIKE 'test'") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT * FROM users WHERE emails LIKE 'test';";
        REQUIRE(run(g, in) == true);
    }

    SECTION("SELECT * FROM test WHERE NOT EXISTS (SELECT TRUE)") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT * FROM test WHERE NOT EXISTS (SELECT TRUE);";
        REQUIRE(run(g, in) == true);
    }

    SECTION("SELECT u.* FROM users u WHERE access_token IS NOT NULL") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT u.* FROM users u WHERE access_token IS NOT NULL;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("comment in the beginning") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "-- user rows with login 'nehzata'\nSELECT u.*\n ;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("comment in the middle") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT u.* -- all user columns\nFROM users --\nWHERE TRUE;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("join") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in =
            "SELECT r.name "
            "FROM repos r "
            "JOIN user_repos ur ON ur.repo_type = r.repo_type AND ur.repo_gid = r.repo_gid "
            "WHERE ur.user_id = 123;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("error") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in =
            "SELECT r.repo_gid, d.dependee_gid "
            "FROM repos r "
            "JOIN repo_dependencies d ON r.repo_gid = d.depender_gid "
            "WHERE r.name = 'thanksd' AND d.dependency_type = 'GH'";
        REQUIRE(run(g, in) == true);
    }
    spdlog::set_level(spdlog::level::off);
}
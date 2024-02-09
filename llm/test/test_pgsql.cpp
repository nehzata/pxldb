#include <spdlog/spdlog.h>

#include <catch2/catch_all.hpp>

#include "grammar/pgsql/pgsql.h"
#include "utils.h"

TEST_CASE("pgsql") {
    SECTION("basic query 1") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT TRUE;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 2") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT TRUE FROM test;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 3") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT * FROM my_table;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 4") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT * FROM users WHERE emails LIKE 'test';";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 5") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT * FROM test WHERE NOT EXISTS (SELECT TRUE);";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 6") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT users.id AS user_id FROM users;";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 7") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT last_ts < (1234) FROM users";
        REQUIRE(run(g, in) == true);
    }

    SECTION("basic query 8") {
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
        REQUIRE(run(g, in) != true);
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

    SECTION("join complicated") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in =
            "SELECT r.repo_gid, d.dependee_gid "
            "FROM repos r "
            "JOIN repo_dependencies d ON r.repo_gid = d.depender_gid "
            "WHERE r.name = 'thanksd' AND d.dependency_type = 'GH'";
        REQUIRE(run(g, in) == true);
    }

    SECTION("functions 1") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT NOW()";
        REQUIRE(run(g, in) == true);
    }

    SECTION("functions 2") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT MAX(last_ts) FROM users";
        REQUIRE(run(g, in) == true);
    }

    SECTION("functions 3") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT EXTRACT(EPOCH FROM NOW())";
        REQUIRE(run(g, in) == true);
    }

    SECTION("functions 4") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT DATE_PART('century', NOW())";
        REQUIRE(run(g, in) == true);
    }

    SECTION("incomplete query 1") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in = "SELECT MAX(";
        REQUIRE(run(g, in) != true);
    }

    SECTION("incomplete query 2") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_pgsql g;
        std::string in =
            "-- join users and user_keys  table based on user_id\n"
            " SELECT u.id, u.email, u.username, uk.key\n"
            " FROM users u\n"
            " INNER JOIN user_keys uk ON u.id = uk.user_id\n"
            " WHERE";
        REQUIRE(run(g, in) != true);
    }

    spdlog::set_level(spdlog::level::off);
}
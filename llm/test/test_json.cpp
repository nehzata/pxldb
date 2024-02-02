// Let Catch2 know that we are using its main function.
#include <spdlog/spdlog.h>

#include <catch2/catch_all.hpp>

#include "grammar/json/json.h"
#include "utils.h"

TEST_CASE("json") {
    SECTION("null") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = "null";
        REQUIRE(run(g, in) == true);
    }
    SECTION("boolean:true") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = "true";
        REQUIRE(run(g, in) == true);
    }
    SECTION("boolean:false") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = "false";
        REQUIRE(run(g, in) == true);
    }
    SECTION("number") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = "123";
        REQUIRE(run(g, in) == true);
    }
    SECTION("string") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = R"("string")";
        REQUIRE(run(g, in) == true);
    }
    SECTION("array:empty") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = R"([])";
        REQUIRE(run(g, in) == true);
    }
    SECTION("array") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = R"(["string",123, null,false])";
        REQUIRE(run(g, in) == true);
    }
    SECTION("object:empty") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = R"({})";
        REQUIRE(run(g, in) == true);
    }
    SECTION("object") {
        // spdlog::set_level(spdlog::level::debug);
        grammar_json g;
        std::string in = R"({"test":true, "value":1, "empty" : "no"})";
        REQUIRE(run(g, in) == true);
    }

    spdlog::set_level(spdlog::level::off);
}
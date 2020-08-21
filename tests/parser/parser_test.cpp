#include "catch2/catch.hpp"
#include "parser.hpp"

SCENARIO("Neutral storyline") {
    GIVEN("Magic performed") {
        int a = 0;
        magic();

        THEN("Nothing happens") {
            REQUIRE(a == 0);
        }
    }
}

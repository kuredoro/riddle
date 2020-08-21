#include "catch2/catch.hpp"
#include "lexer.hpp"

SCENARIO("Summing two numbers") {
    // By the way, don't mention constants in descriptions
    GIVEN("Two positive numbers: 2 and 3") {
        int a = 2, b = 3;

        THEN("their sum is 5") {
            REQUIRE(sum(a, b) == 5);
        }
    }
}

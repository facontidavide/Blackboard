#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "SafeAny/safe_any.hpp"


TEST_CASE( "Basic", "Any" )
{
    using VarNumber::Any;

    REQUIRE( Any(int(250)).convert<uint8_t>() == 250 );

    REQUIRE_THROWS( Any(int(300)).convert<uint8_t>() );

    REQUIRE_THROWS( Any(int(-1)).convert<uint8_t>() );

    REQUIRE( Any(bool(true)).convert<uint8_t>() == 1 );

    REQUIRE( Any(char(100)).convert<uint8_t>() == 100 );
}

TEST_CASE( "String", "Any" )
{
    using VarNumber::Any;

    std::string hello("Hello");

    REQUIRE( Any(int(250)).convert<std::string>() == "250" );

    REQUIRE( Any(hello).extract<std::string>() == hello);

}

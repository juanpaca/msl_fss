#include <catch2/catch.hpp>

#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

#include <cstdio>
#include <string>

using namespace msl_fss;

TEST_CASE("Config get/set and defaults", "[unit][config]") {
    Config cfg;
    cfg.set("alpha", "1.5");
    cfg.set("numelems", "10");
    REQUIRE(cfg.has("alpha"));
    REQUIRE(cfg.getDouble("alpha", 0.0) == Approx(1.5));
    REQUIRE(cfg.getInt("numelems", 0) == 10);
    REQUIRE(cfg.getDouble("missing", 3.5) == Approx(3.5));
    REQUIRE_FALSE(cfg.has("missing"));
}

TEST_CASE("Config parses a trivial key=value file", "[unit][config]") {
    const char* path = "config_test.ini";
    std::FILE* f = std::fopen(path, "w");
    REQUIRE(f != nullptr);
    std::fputs("alpha = 2.0   # Biot-Willis\n", f);
    std::fputs("Kdr = 4.0\n", f);
    std::fputs("ndof = 5\n", f);
    std::fclose(f);

    Config cfg = Config::fromFile(path);
    REQUIRE(cfg.getDouble("alpha", 0.0) == Approx(2.0));
    REQUIRE(cfg.getDouble("Kdr", 0.0) == Approx(4.0));
    REQUIRE(cfg.getInt("ndof", 0) == 5);
}

TEST_CASE("Field arithmetic", "[unit][field]") {
    Field a(3);
    a[0] = 1.0; a[1] = 2.0; a[2] = 3.0;
    Field b(3);
    b[0] = 0.5; b[1] = 1.0; b[2] = 1.5;

    Field d = a - b;
    REQUIRE(d[0] == Approx(0.5));
    REQUIRE(d[1] == Approx(1.0));
    REQUIRE(d[2] == Approx(1.5));

    REQUIRE_THROWS_AS(Field(2) - Field(3), std::invalid_argument);
}
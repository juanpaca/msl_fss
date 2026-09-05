#include <catch2/catch.hpp>

#include "msl_fss/Infrastructure/LinearSystem.hpp"

using namespace msl_fss;

TEST_CASE("LinearSystem dummy solve with identity matrix returns rhs",
          "[unit][linearsystem]") {
    DenseMatrix A(3, 3);
    for (std::size_t i = 0; i < 3; ++i) A(i, i) = 1.0;

    Field b(3);
    b[0] = 2.0; b[1] = -1.0; b[2] = 0.5;

    LinearSystem sys;
    sys.SetMatrix(A);
    sys.SetRHS(b);
    Field x = sys.Solve();

    REQUIRE(sys.isFactorized());
    REQUIRE(x.size() == 3);
    REQUIRE(x[0] == Approx(2.0));
    REQUIRE(x[1] == Approx(-1.0));
    REQUIRE(x[2] == Approx(0.5));
}

TEST_CASE("LinearSystem factorize is idempotent", "[unit][linearsystem]") {
    LinearSystem sys;
    DenseMatrix A(2, 2);
    A(0, 0) = 1.0; A(1, 1) = 1.0;
    sys.SetMatrix(A);
    sys.Factorize();
    REQUIRE(sys.isFactorized());
    sys.Factorize();
    REQUIRE(sys.isFactorized());
}
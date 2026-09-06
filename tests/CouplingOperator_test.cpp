#include <catch2/catch.hpp>

#include "msl_fss/Infrastructure/CouplingOperator.hpp"

using namespace msl_fss;

TEST_CASE("CouplingOperator applies pressure to mechanics via alpha*P",
          "[unit][coupling]") {
    const double alpha = 2.0, Kdr = 5.0;
    CouplingOperator op(alpha, Kdr);

    Field p(3);
    p[0] = 1.0; p[1] = 2.0; p[2] = 3.0;

    Field load = op.ApplyPressureToMech(p);

    REQUIRE(load.size() == 3);
    REQUIRE(load[0] == Approx(alpha * 1.0));
    REQUIRE(load[1] == Approx(alpha * 2.0));
    REQUIRE(load[2] == Approx(alpha * 3.0));
}

TEST_CASE("CouplingOperator computes sigmav = Kdr*div(u) - alpha*p",
          "[unit][coupling]") {
    const double alpha = 2.0, Kdr = 5.0;
    CouplingOperator op(alpha, Kdr);

    Field u(4);
    u[0] = 1.0; u[1] = 1.0; u[2] = 1.0; u[3] = 1.0;
    Field p(4);
    p[0] = 1.0; p[1] = 0.0; p[2] = 0.5; p[3] = 0.25;

    Field sig = op.ComputeVolumetricStress(u, p);

    REQUIRE(sig.size() == 4);
    for (std::size_t i = 0; i < 4; ++i) {
        REQUIRE(sig[i] == Approx(Kdr * u[i] - alpha * p[i]));
    }
}

TEST_CASE("CouplingOperator exposes alpha and Kdr", "[unit][coupling]") {
    CouplingOperator op(1.5, 7.0);
    REQUIRE(op.alpha() == Approx(1.5));
    REQUIRE(op.Kdr() == Approx(7.0));
}

TEST_CASE("CouplingOperator provides the divergence matrix D_K",
          "[unit][coupling]") {
    const double alpha = 2.0;
    CouplingOperator op(alpha, 5.0);

    Field p(4);
    p[0] = 1.0; p[1] = 2.0; p[2] = 3.0; p[3] = 4.0;

    // Dummy D_K = I: D^T * p == p, independent of the macro-element K.
    REQUIRE(op.GetDivMatrix(0).mul(p.data()) == p.data());
    REQUIRE(op.GetDivMatrix(7).mul(p.data()) == p.data());

    // ApplyPressureToMech is the alpha-scaled D^T * P application.
    Field load = op.ApplyPressureToMech(p);
    for (std::size_t i = 0; i < p.size(); ++i) {
        REQUIRE(load[i] == Approx(alpha * p[i]));
    }
}
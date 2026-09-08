#include <catch2/catch.hpp>

#include "msl_fss/Control/FixedStressController.hpp"
#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

#include <stdexcept>
#include <limits>

using namespace msl_fss;

namespace {

// Minimal subproblem contract exercised by the controller: Setup + Offline +
// Online (the run method only touches those, no domain knowledge required).
struct IdentitySub {
    Field Online(const Field& rhs) { return rhs; }
    void Setup(SharedMHMHierarchy*) {}
    void Offline() {}
};

// Flow that alternates its output on every Online() call: the oscillating
// pressure keeps a large relative error forever, so the FSS loop can only
// terminate through the MAX_ITER branch.
struct AlternatingFlow {
    Field Online(const Field&) {
        flip_ = !flip_;
        Field f(4);
        for (std::size_t i = 0; i < f.size(); ++i) f[i] = flip_ ? 1.0 : -1.0;
        return f;
    }
    void Setup(SharedMHMHierarchy*) {}
    void Offline() {}
    bool flip_ = false;
};

struct DivergingFlow {
    Field Online(const Field&) {
        Field f(4);
        f[0] = std::numeric_limits<double>::quiet_NaN();
        return f;
    }
    void Setup(SharedMHMHierarchy*) {}
    void Offline() {}
};

} // namespace

TEST_CASE("FixedStressController Configure reads the FSS parameters",
          "[unit][fss]") {
    Config cfg;
    cfg.set("ftol", "1e-4");
    cfg.set("fmax_iter", "7");
    cfg.set("fL", "2.5");
    cfg.set("numelems", "5");

    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    IdentitySub flow, mech;
    FixedStressController<IdentitySub, IdentitySub> controller(flow, mech,
                                                               coupling, hierarchy);

    REQUIRE_FALSE(controller.isReady());
    controller.Configure(cfg);
    REQUIRE(controller.isReady());
    REQUIRE(controller.tolerance() == Approx(1e-4));
    REQUIRE(controller.maxIterations() == 7);
    REQUIRE(controller.stabilization() == Approx(2.5));
    REQUIRE(controller.hierarchy().GetPartition().numElements == 5);
}

TEST_CASE("FixedStressController requires Configure before RunTimeStep",
          "[unit][fss]") {
    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    IdentitySub flow, mech;
    FixedStressController<IdentitySub, IdentitySub> controller(flow, mech,
                                                               coupling, hierarchy);

    REQUIRE_THROWS_AS(controller.RunTimeStep(0.1), std::runtime_error);
}

TEST_CASE("FixedStressController RelativeError computes the relative norm",
          "[unit][fss]") {
    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    IdentitySub flow, mech;
    FixedStressController<IdentitySub, IdentitySub> controller(flow, mech,
                                                               coupling, hierarchy);

    Field prev(4);
    prev[0] = 1.0; prev[1] = 2.0; prev[2] = 3.0; prev[3] = 4.0;
    Field curr(4);
    curr[0] = 2.0; curr[1] = 4.0; curr[2] = 6.0; curr[3] = 8.0;

    REQUIRE(controller.RelativeError(prev, prev) == Approx(0.0));
    // ||curr - prev|| / ||curr|| = sqrt(30 / 120) = 0.5
    REQUIRE(controller.RelativeError(prev, curr) == Approx(0.5));
}

TEST_CASE("FixedStressController converges on the dummy identity path",
          "[unit][fss]") {
    Config cfg;
    cfg.set("alpha", "1.0");
    cfg.set("Kdr", "2.0");

    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    IdentitySub flow, mech;
    FixedStressController<IdentitySub, IdentitySub> controller(flow, mech,
                                                               coupling, hierarchy);

    controller.Configure(cfg);
    REQUIRE(controller.RunTimeStep(0.1) == FSSStatus::CONVERGED);
}

TEST_CASE("FixedStressController terminates by MAX_ITER with zero iterations",
          "[unit][fss]") {
    Config cfg;
    cfg.set("alpha", "1.0");
    cfg.set("Kdr", "1.0");
    cfg.set("fmax_iter", "0");

    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    IdentitySub flow, mech;
    FixedStressController<IdentitySub, IdentitySub> controller(flow, mech,
                                                               coupling, hierarchy);

    controller.Configure(cfg);
    REQUIRE(controller.RunTimeStep(0.1) == FSSStatus::MAX_ITER);
}

TEST_CASE("FixedStressController without convergence terminates by MAX_ITER",
          "[unit][fss]") {
    Config cfg;
    cfg.set("alpha", "1.0");
    cfg.set("Kdr", "2.0");
    cfg.set("ftol", "1e-9");
    cfg.set("fmax_iter", "10");

    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    AlternatingFlow flow;
    IdentitySub mech;
    FixedStressController<AlternatingFlow, IdentitySub> controller(flow, mech,
                                                                   coupling, hierarchy);

    controller.Configure(cfg);
    // The oscillating pressure never satisfies the relative-error test, so the
    // loop runs up to kMax_ and the status is MAX_ITER.
    REQUIRE(controller.RunTimeStep(0.1) == FSSStatus::MAX_ITER);
}

TEST_CASE("FixedStressController reports DIVERGED for non-finite fields",
           "[unit][fss]") {
    Config cfg;
    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(1.0, 2.0);
    DivergingFlow flow;
    IdentitySub mech;
    FixedStressController<DivergingFlow, IdentitySub> controller(flow, mech,
                                                                  coupling, hierarchy);

    controller.Configure(cfg);
    REQUIRE(controller.RunTimeStep(0.1) == FSSStatus::DIVERGED);
}

#include <catch2/catch.hpp>

#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"

using namespace msl_fss;

// Concrete CRTP-derived types used by these tests (mirror the mains).
struct FlowGal final : FlowGalerkin<FlowGal> {};
struct MechGal final : MechanicsGalerkin<MechGal> {};
struct FlowMHMT final : FlowMHM<FlowMHMT> {};
struct MechMHMT final : MechanicsMHM<MechMHMT> {};

TEST_CASE("SubproblemSolver (Galerkin) runs offline/online", "[unit][subproblem]") {
    FlowGal flow;
    flow.Setup(nullptr);
    REQUIRE(flow.isSetup());
    flow.Offline();
    REQUIRE(flow.isOffline());

    Field rhs(4);
    rhs[0] = 1.0; rhs[1] = 2.0; rhs[2] = 3.0; rhs[3] = 4.0;
    Field sol = flow.Online(rhs);
    REQUIRE(sol.size() == 4);
    // dummy identity solve
    REQUIRE(sol[0] == Approx(1.0));
    REQUIRE(sol[3] == Approx(4.0));
}

TEST_CASE("Physical axis wraps facade with correct traits", "[unit][subproblem]") {
    Config cfg;
    cfg.set("alpha", "2.5");
    cfg.set("Kdr", "3.0");
    cfg.set("ndof", "6");

    FlowPhysics flow;
    flow.Setup(cfg);
    REQUIRE(flow.isConfigured());
    REQUIRE(flow.facade().alpha() == Approx(2.5));
    REQUIRE(flow.facade().Kdr() == Approx(3.0));
    REQUIRE(flow.facade().size() == 6);
    REQUIRE(FlowPhysics::kTransient == true);
    REQUIRE(FlowPhysics::kVectorial == false);

    MechanicsPhysics mech;
    mech.Setup(cfg);
    REQUIRE(mech.isConfigured());
    REQUIRE(MechanicsPhysics::kTransient == false);
    REQUIRE(MechanicsPhysics::kVectorial == true);
}

TEST_CASE("MultiscaleSolver materializes the skeleton contributions",
          "[unit][subproblem]") {
    Config cfg;
    cfg.set("numelems", "4");
    cfg.set("ndof_local", "3");

    SharedMHMHierarchy hierarchy;
    hierarchy.Build(cfg);

    FlowMHMT flow;
    flow.Setup(&hierarchy);
    flow.Offline();
    REQUIRE(flow.isOffline());

    flow.resolver().GetSkeletonContributions();
    REQUIRE(flow.resolver().isSkeletonReady());
    REQUIRE(flow.resolver().skeletonContribs().size() == hierarchy.numSubMeshes());
    for (std::size_t K = 0; K < flow.resolver().skeletonContribs().size(); ++K) {
        REQUIRE(flow.resolver().skeletonContribs()[K].rows() == 2);
    }
}
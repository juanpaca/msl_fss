#include <catch2/catch.hpp>

#include "msl_fss/Control/FixedStressController.hpp"
#include "msl_fss/Control/PoromechanicsMFEMProblem.hpp"
#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"

using namespace msl_fss;

struct FlowGal final : FlowGalerkin<FlowGal> {};
struct MechGal final : MechanicsGalerkin<MechGal> {};
struct FlowMHMT final : FlowMHM<FlowMHMT> {};
struct MechMHMT final : MechanicsMHM<MechMHMT> {};

TEST_CASE("FixedStressController converges with Galerkin subproblems",
          "[integration][fss]") {
    Config cfg;
    cfg.set("alpha", "1.0");
    cfg.set("Kdr", "2.0");

    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(cfg.getDouble("alpha", 1.0),
                              cfg.getDouble("Kdr", 1.0));
    FlowGal flow;
    MechGal mech;
    FixedStressController<FlowGal, MechGal> controller(flow, mech, coupling,
                                                       hierarchy);
    controller.Configure(cfg);

    FSSStatus status = controller.RunTimeStep(0.1);
    REQUIRE(status == FSSStatus::CONVERGED);
}

TEST_CASE("FixedStressController converges with MHM subproblems and shared hierarchy",
          "[integration][fss][mhm]") {
    Config cfg;
    cfg.set("alpha", "1.0");
    cfg.set("Kdr", "2.0");
    cfg.set("numelems", "4");

    SharedMHMHierarchy hierarchy;
    CouplingOperator coupling(cfg.getDouble("alpha", 1.0),
                              cfg.getDouble("Kdr", 1.0));
    FlowMHMT flow;
    MechMHMT mech;
    FixedStressController<FlowMHMT, MechMHMT> controller(flow, mech, coupling,
                                                         hierarchy);
    controller.Configure(cfg);

    FSSStatus status = controller.RunTimeStep(0.1);
    REQUIRE(status == FSSStatus::CONVERGED);

    // The MHM path populated the shared hierarchy and the offline store.
    REQUIRE(hierarchy.GetPartition().numElements == 4);
    REQUIRE(flow.resolver().store().Size() > 0);
    REQUIRE(mech.resolver().store().Size() > 0);
}

TEST_CASE("PoromechanicsMFEMProblem facade runs and exports",
          "[integration][facade]") {
    Config cfg;
    cfg.set("alpha", "1.0");
    cfg.set("Kdr", "2.0");

    PoromechanicsMFEMProblem facade;
    facade.Run(cfg);
    REQUIRE(facade.ran());
    facade.ExportResults();
    REQUIRE(facade.exported());
    REQUIRE(facade.alpha() == Approx(1.0));
}
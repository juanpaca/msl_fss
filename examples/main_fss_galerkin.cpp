#include "msl_fss/Control/FixedStressController.hpp"
#include "msl_fss/Control/PoromechanicsMFEMProblem.hpp"
#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"

#include <iostream>

// Concrete CRTP-derived types of the *Galerkin* strategy (validation
// reference). Each is a SubproblemSolver specialized in both the physical axis
// (flow/mechanics) and the monoscale resolvedor axis (GalerkinSolver).
struct FlowGal final : msl_fss::FlowGalerkin<FlowGal> {};
struct MechGal final : msl_fss::MechanicsGalerkin<MechGal> {};

// Entry point of the Galerkin strategy. Both subproblems are solved by
// classical finite elements on the fine mesh; the offline phase merely
// assembles and factorizes the global operators once, and each FSS iteration
// only updates the right-hand sides (online phase).
int main(int argc, char* argv[]) {
    const std::string conf_path =
        (argc > 1) ? argv[1] : "config_fss_galerkin.ini";
    msl_fss::Config config = msl_fss::Config::fromFile(conf_path);

    FlowGal flow;
    MechGal mech;

    msl_fss::SharedMHMHierarchy hierarchy;
    msl_fss::CouplingOperator coupling(config.getDouble("alpha", 1.0),
                                       config.getDouble("Kdr", 1.0));

    msl_fss::FixedStressController<FlowGal, MechGal> controller(flow, mech,
                                                                coupling,
                                                                hierarchy);
    controller.Configure(config);
    msl_fss::FSSStatus status = controller.RunTimeStep(config.getDouble("dt", 0.1));

    msl_fss::PoromechanicsMFEMProblem facade;
    facade.Run(config);
    facade.ExportResults();

    std::cout << "main_fss_galerkin: strategy="
              << static_cast<int>(msl_fss::SolverStrategy::GALERKIN)
              << " status=" << static_cast<int>(status)
              << " ran=" << facade.ran()
              << " exported=" << facade.exported() << "\n";
    return 0;
}
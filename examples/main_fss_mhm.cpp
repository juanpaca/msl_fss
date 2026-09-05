#include "msl_fss/Control/FixedStressController.hpp"
#include "msl_fss/Control/PoromechanicsMFEMProblem.hpp"
#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"

#include <iostream>

// Concrete CRTP-derived types of the *MHM* strategy (productive path). Each is
// a SubproblemSolver specialized in the physical axis and in the multiscale
// resolvedor axis (MultiscaleSolver, using the MHMGlobalProblem and the shared
// geometry hierarchy).
struct FlowMHM_concrete final : msl_fss::FlowMHM<FlowMHM_concrete> {};
struct MechMHM_concrete final : msl_fss::MechanicsMHM<MechMHM_concrete> {};

// Entry point of the MHM strategy. The offline phase is richer: it partitions
// the domain, builds the shared geometric hierarchy (splitToDisk), the fine
// local sub-meshes, the Lambda_H multiplier spaces, the flux bases and the
// global skeleton problems, factorizing local and global operators.
int main(int argc, char* argv[]) {
    const std::string conf_path =
        (argc > 1) ? argv[1] : "config_fss_mhm.ini";
    msl_fss::Config config = msl_fss::Config::fromFile(conf_path);

    // The shared hierarchy is built once and reused by both subproblems (FR2).
    msl_fss::SharedMHMHierarchy hierarchy;

    FlowMHM_concrete flow;
    MechMHM_concrete mech;

    msl_fss::CouplingOperator coupling(config.getDouble("alpha", 1.0),
                                       config.getDouble("Kdr", 1.0));

    msl_fss::FixedStressController<FlowMHM_concrete, MechMHM_concrete>
        controller(flow, mech, coupling, hierarchy);
    controller.Configure(config);
    msl_fss::FSSStatus status = controller.RunTimeStep(config.getDouble("dt", 0.1));

    msl_fss::PoromechanicsMFEMProblem facade;
    facade.Run(config);
    facade.ExportResults();

    std::cout << "main_fss_mhm: strategy="
              << static_cast<int>(msl_fss::SolverStrategy::MHM)
              << " status=" << static_cast<int>(status)
              << " numElems=" << hierarchy.GetPartition().numElements
              << " ran=" << facade.ran()
              << " exported=" << facade.exported() << "\n";
    return 0;
}
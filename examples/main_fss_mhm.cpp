#include "msl_fss/Control/FixedStressController.hpp"
#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"

#include <iostream>

// Concrete CRTP-derived types for the dummy MHM-shaped strategy. The real MHM
// integration remains a planned adapter to the MSL stack.
struct FlowMHM_concrete final : msl_fss::FlowMHM<FlowMHM_concrete> {};
struct MechMHM_concrete final : msl_fss::MechanicsMHM<MechMHM_concrete> {};

// Entry point that validates the MHM-shaped control flow with dummy operators.
int main(int argc, char* argv[]) {
    const std::string conf_path =
        (argc > 1) ? argv[1] : "config_fss_mhm.ini";
    msl_fss::Config config = msl_fss::Config::fromFile(conf_path);

    FlowMHM_concrete flow;
    MechMHM_concrete mech;
    msl_fss::SharedMHMHierarchy hierarchy;
    msl_fss::CouplingOperator coupling(config.getDouble("alpha", 1.0),
                                       config.getDouble("Kdr", 1.0));
    msl_fss::FixedStressController<FlowMHM_concrete, MechMHM_concrete> controller(
        flow, mech, coupling, hierarchy);
    controller.Configure(config);

    msl_fss::FSSStatus status = msl_fss::FSSStatus::CONVERGED;
    const double dt = config.getDouble("dt", 0.1);
    for (int n = 0; n < config.getInt("num_steps", 1); ++n) {
        controller.BeginTimeStep(dt);
        for (int k = 0; k < controller.maxIterations(); ++k) {
            status = controller.RunFixedStressIteration();
            if (status != msl_fss::FSSStatus::ITERATING) break;
        }
        if (status == msl_fss::FSSStatus::ITERATING)
            status = msl_fss::FSSStatus::MAX_ITER;
        if (status != msl_fss::FSSStatus::CONVERGED) break;
    }

    std::cout << "main_fss_mhm: strategy="
               << static_cast<int>(msl_fss::Strategy::MHM)
               << " status=" << static_cast<int>(status)
               << " numElems=" << hierarchy.GetPartition().numElements
               << " time_steps=" << config.getInt("num_steps", 1) << "\n";
    return status == msl_fss::FSSStatus::CONVERGED ? 0 : 1;
}

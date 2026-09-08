#include "msl_fss/Control/FixedStressController.hpp"
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

// Entry point that validates the Galerkin-shaped control flow with dummy
// operators; it is not an MFEM finite-element solve.
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
                                                                 coupling, hierarchy);
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

    std::cout << "main_fss_galerkin: strategy="
               << static_cast<int>(msl_fss::Strategy::GALERKIN)
              << " status=" << static_cast<int>(status)
              << " time_steps=" << config.getInt("num_steps", 1) << "\n";
    return status == msl_fss::FSSStatus::CONVERGED ? 0 : 1;
}

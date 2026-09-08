#include "msl_fss/Control/PoromechanicsMFEMProblem.hpp"
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

    msl_fss::PoromechanicsMFEMProblem<FlowMHM_concrete, MechMHM_concrete> facade;
    msl_fss::FSSStatus status = facade.Run(config);
    if (status == msl_fss::FSSStatus::CONVERGED) facade.ExportResults();

    std::cout << "main_fss_mhm: strategy="
               << static_cast<int>(msl_fss::Strategy::MHM)
               << " status=" << static_cast<int>(status)
               << " numElems=" << facade.hierarchy().GetPartition().numElements
              << " ran=" << facade.ran()
              << " exported=" << facade.exported() << "\n";
    return status == msl_fss::FSSStatus::CONVERGED ? 0 : 1;
}

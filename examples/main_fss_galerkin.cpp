#include "msl_fss/Control/PoromechanicsMFEMProblem.hpp"
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

    msl_fss::PoromechanicsMFEMProblem<FlowGal, MechGal> facade;
    msl_fss::FSSStatus status = facade.Run(config);
    if (status == msl_fss::FSSStatus::CONVERGED) facade.ExportResults();

    std::cout << "main_fss_galerkin: strategy="
               << static_cast<int>(msl_fss::Strategy::GALERKIN)
              << " status=" << static_cast<int>(status)
              << " ran=" << facade.ran()
              << " exported=" << facade.exported() << "\n";
    return status == msl_fss::FSSStatus::CONVERGED ? 0 : 1;
}

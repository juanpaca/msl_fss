#include "msl_fss/Control/PoromechanicsMFEMProblem.hpp"

namespace msl_fss {

void PoromechanicsMFEMProblem::Run(const Config& config) {
    config_ = config;
    alpha_ = config.getDouble("alpha", 1.0);
    kdr_ = config.getDouble("Kdr", 1.0);

    // Dummy: the real product reads the mesh, builds the shared hierarchy,
    // runs the offline phase of each subproblem and the FSS time loop.
    // Here we only record the run for the tests.
    ran_ = true;
}

void PoromechanicsMFEMProblem::ExportResults() {
    // Dummy: real export writes .vtu grid functions through MFEM. Here we only
    // record that the call happened.
    exported_ = true;
}

} // namespace msl_fss
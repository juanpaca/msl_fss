#ifndef MSL_FSS_CONTROL_POROMECHANICSMFEMPROBLEM_HPP
#define MSL_FSS_CONTROL_POROMECHANICSMFEMPROBLEM_HPP

#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

namespace msl_fss {

// High-level facade of the solver (THEALO). Orchestrates the reading of the
// Lua/MSL configuration, the construction of the mesh, and the instantiation
// of the FixedStressController with the typed subproblems, delegating the
// execution. It keeps the external interface of the solver separated from the
// temporal coupling.
class PoromechanicsMFEMProblem {
public:
    // Reads the config (MSL/Lua), builds the typed controller and the shared
    // hierarchy, runs the offline phase and the FSS time loop.
    void Run(const Config& config);

    // Exports the pressure and displacement fields to .vtu (dummy: no-op that
    // records the call, since the real ParaView export lives in the MFEM
    // grid-function output).
    void ExportResults();

    // --- accessors used by the tests --------------------------------------
    bool ran() const { return ran_; }
    bool exported() const { return exported_; }
    const Config& config() const { return config_; }
    double alpha() const { return alpha_; }
    double Kdr() const { return kdr_; }

private:
    Config config_;
    double alpha_ = 1.0;
    double kdr_ = 1.0;
    bool ran_ = false;
    bool exported_ = false;
};

} // namespace msl_fss

#endif // MSL_FSS_CONTROL_POROMECHANICSMFEMPROBLEM_HPP
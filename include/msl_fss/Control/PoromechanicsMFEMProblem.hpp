#ifndef MSL_FSS_CONTROL_POROMECHANICSMFEMPROBLEM_HPP
#define MSL_FSS_CONTROL_POROMECHANICSMFEMPROBLEM_HPP

#include "msl_fss/Control/FixedStressController.hpp"
#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/SubproblemSolver.hpp"
#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

#include <optional>
#include <stdexcept>

namespace msl_fss {

// High-level dummy facade. It validates typed-controller orchestration while
// the real Lua/MSL configuration and MFEM mesh adapters remain future work.
template <typename FlowSub, typename MechSub>
class PoromechanicsMFEMProblem {
public:
    // Reads the dummy key-value config, builds the typed controller and runs
    // the placeholder offline phase and FSS control loop.
    FSSStatus Run(const Config& config) {
        config_ = config;
        alpha_ = config.getDouble("alpha", 1.0);
        kdr_ = config.getDouble("Kdr", 1.0);

        flow_.Setup(config_);
        mech_.Setup(config_);
        coupling_.emplace(alpha_, kdr_);
        controller_.emplace(flow_, mech_, *coupling_, hierarchy_);
        controller_->Configure(config_);

        status_ = FSSStatus::CONVERGED;
        const int numSteps = config_.getInt("num_steps", 1);
        for (int n = 0; n < numSteps; ++n) {
            status_ = controller_->RunTimeStep(config_.getDouble("dt", 0.1));
            if (status_ != FSSStatus::CONVERGED) {
                ran_ = false;
                return status_;
            }
        }
        ran_ = true;
        return status_;
    }

    // Exports the pressure and displacement fields to .vtu (dummy: no-op that
    // records the call, since the real ParaView export lives in the MFEM
    // grid-function output).
    void ExportResults() {
        if (!ran_) throw std::logic_error(
            "PoromechanicsMFEMProblem::ExportResults requires a converged Run()");
        exported_ = true;
    }

    // --- accessors used by the tests --------------------------------------
    bool ran() const { return ran_; }
    bool exported() const { return exported_; }
    const Config& config() const { return config_; }
    double alpha() const { return alpha_; }
    double Kdr() const { return kdr_; }
    FSSStatus status() const { return status_; }
    const SharedMHMHierarchy& hierarchy() const { return hierarchy_; }

private:
    Config config_;
    FlowSub flow_;
    MechSub mech_;
    SharedMHMHierarchy hierarchy_;
    std::optional<CouplingOperator> coupling_;
    std::optional<FixedStressController<FlowSub, MechSub>> controller_;
    double alpha_ = 1.0;
    double kdr_ = 1.0;
    FSSStatus status_ = FSSStatus::MAX_ITER;
    bool ran_ = false;
    bool exported_ = false;
};

} // namespace msl_fss

#endif // MSL_FSS_CONTROL_POROMECHANICSMFEMPROBLEM_HPP

#ifndef MSL_FSS_CONTROL_FIXEDSTRESSCONTROLLER_HPP
#define MSL_FSS_CONTROL_FIXEDSTRESSCONTROLLER_HPP

#include "msl_fss/Infrastructure/CouplingOperator.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace msl_fss {

// Orchestrates the Fixed-Stress Split iteration (FR6).
//
// It is a template parametrized by the two concrete subproblem types, so the
// controller knows the concrete types at compile time and performs no virtual
// dispatch in the FSS loop. The coupling is transversal and stays in a single
// CouplingOperator instance shared by both subproblems (FR5).
//
// The controller follows the sequence of the execution view: it builds the
// shared hierarchy once (FR2), configures and runs the offline phase of each
// subproblem, and then iterates the FSS loop (flow -> coupling -> mechanics ->
// volumetric stress -> convergence check).
template <typename FlowSub, typename MechSub>
class FixedStressController {
public:
    FixedStressController(FlowSub& flow, MechSub& mech,
                          const CouplingOperator& coupling,
                          SharedMHMHierarchy& hierarchy)
        : flow_(flow), mech_(mech), coupling_(coupling), hierarchy_(hierarchy) {}

    void setTolerance(double tol) { tolerance_ = tol; }
    void setMaxIterations(int kMax) { kMax_ = kMax; }
    void setStabilization(double L) { L_ = L; }

    double tolerance() const { return tolerance_; }
    int maxIterations() const { return kMax_; }
    double stabilization() const { return L_; }

    // Reads the FSS parameters from config, builds the shared hierarchy and
    // runs the offline phase (assembly/factorization) of both subproblems.
    void Configure(const Config& config) {
        setTolerance(config.getDouble("ftol", tolerance_));
        setMaxIterations(config.getInt("fmax_iter", kMax_));
        setStabilization(config.getDouble("fL", L_));

        hierarchy_.Build(config);
        flow_.Setup(&hierarchy_);
        mech_.Setup(&hierarchy_);
        flow_.Offline();
        mech_.Offline();
        ready_ = true;
    }

    // Executes one complete time step (dt), returning the FSS status. Requires
    // Configure() to have been called.
    FSSStatus RunTimeStep(double dt);

    // Relative error in the (dummy) Euclidean norm between prev and curr,
    // used by the H1 convergence test (\eqref{eq:convergence}).
    double RelativeError(const Field& prev, const Field& curr) const;

    bool isReady() const { return ready_; }
    const SharedMHMHierarchy& hierarchy() const { return hierarchy_; }

private:
    Field SolveFlow(const Field& sigmaV);
    Field SolveMechanics(const Field& pressure);
    bool CheckConvergence(const Field& prev, const Field& curr, double tol) const;

    FlowSub& flow_;
    MechSub& mech_;
    const CouplingOperator& coupling_;
    SharedMHMHierarchy& hierarchy_;

    double tolerance_ = 1e-6;
    int kMax_ = 100;
    double L_ = 1.0;
    bool ready_ = false;
};

// --- inlined dummy implementation -----------------------------------------

template <typename FlowSub, typename MechSub>
double FixedStressController<FlowSub, MechSub>::RelativeError(
    const Field& prev, const Field& curr) const {
    const Field diff = curr - prev;
    double num = 0.0, den = 0.0;
    for (std::size_t i = 0; i < curr.size(); ++i) {
        num += diff[i] * diff[i];
        den += curr[i] * curr[i];
    }
    return std::sqrt(num / (den + 1e-300));
}

template <typename FlowSub, typename MechSub>
Field FixedStressController<FlowSub, MechSub>::SolveFlow(const Field& sigmaV) {
    // The flow subproblem receives the frozen volumetric stress as the
    // coupling right-hand side term.
    return flow_.Online(sigmaV);
}

template <typename FlowSub, typename MechSub>
Field FixedStressController<FlowSub, MechSub>::SolveMechanics(
    const Field& pressure) {
    // The mechanics subproblem receives the pressure load;
    // the transformed load is computed in the controller coupling step.
    return mech_.Online(pressure);
}

template <typename FlowSub, typename MechSub>
bool FixedStressController<FlowSub, MechSub>::CheckConvergence(
    const Field& prev, const Field& curr, double tol) const {
    return RelativeError(prev, curr) <= tol;
}

template <typename FlowSub, typename MechSub>
FSSStatus FixedStressController<FlowSub, MechSub>::RunTimeStep(double dt) {
    if (!ready_) throw std::runtime_error(
        "FixedStressController::RunTimeStep requires Configure() first");

    // sigmaV initial guess from the previous state (dummy).
    const std::size_t n = 4;
    Field sigmaV(n);
    Field pressure(n);
    Field move(n);

    int k = 0;
    for (; k < kMax_; ++k) {
        Field pressurePrev = pressure;
        Field movePrev = move;

        // Flow step: freeze sigmaV.
        Field pNew = SolveFlow(sigmaV);
        // Coupling: pressure load into mechanics via integration by parts.
        Field mechLoad = coupling_.ApplyPressureToMech(pNew);
        // Mechanics step with the new pressure.
        Field uNew = SolveMechanics(mechLoad);
        // Volumetric stress update for the next flow step.
        Field sigmaVnew = coupling_.ComputeVolumetricStress(uNew, pNew);

        bool conv = CheckConvergence(pressurePrev, pNew, tolerance_)
                 && CheckConvergence(movePrev, uNew, tolerance_);
        if (conv) {
            pressure = pNew;
            move = uNew;
            sigmaV = sigmaVnew;
            return FSSStatus::CONVERGED;
        }
        pressure = pNew;
        move = uNew;
        sigmaV = sigmaVnew;
    }
    (void)dt;
    return k >= kMax_ ? FSSStatus::MAX_ITER : FSSStatus::DIVERGED;
}

} // namespace msl_fss

#endif // MSL_FSS_CONTROL_FIXEDSTRESSCONTROLLER_HPP
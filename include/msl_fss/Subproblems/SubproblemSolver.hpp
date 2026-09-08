#ifndef MSL_FSS_SUBPROBLEMS_SUBPROBLEMSOLVER_HPP
#define MSL_FSS_SUBPROBLEMS_SUBPROBLEMSOLVER_HPP

#include "msl_fss/Infrastructure/ResolverAxis.hpp"
#include "msl_fss/Subproblems/ISubproblemSolver.hpp"
#include "msl_fss/Subproblems/PhysicalAxis.hpp"
#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

namespace msl_fss {

// Central template that combines the two orthogonal axes:
//
//   SubproblemSolver<Physics, Resolver, Self>
//
// where Physics is the *physical axis* (FlowPhysics or MechanicsPhysics) and
// Resolver is the *resolvedor axis* (GalerkinSolver or MultiscaleSolver). The
// third parameter Self is the CRTP-derived concrete type used for static
// dispatch (the same device employed by MFEMProblem<Derived> in the MSL
// stack). All four combinations are instantiations of the same template -- no
// class multiplication.
//
// The Resolver is itself an ISubproblemSolver that owns the offline/online
// logic; the Physics wraps the MSL physical facade (transient scalar for flow,
// stationary vectorial for mechanics).
template <typename Physics, typename Resolver, typename Self>
class SubproblemSolver : public ISubproblemSolver {
public:
    using PhysicsType = Physics;
    using ResolverType = Resolver;

    // CRTP static downcast.
    Self& AsDerived() { return static_cast<Self&>(*this); }
    const Self& AsDerived() const { return static_cast<const Self&>(*this); }

    void Setup(const Config& config) {
        physics_.Setup(config);
        setup_ = true;
    }

    void Setup(SharedMHMHierarchy* hierarchy) override {
        resolver_.Setup(hierarchy);
        setup_ = true;
    }

    void Offline() override {
        physics_.AssembleSystem();
        resolver_.Offline();
        offline_ = true;
    }

    Field Online(const Field& rhs) override {
        // Keep the physical facade in the online path. Both dummy stages are
        // identities, but the call chain matches the future MFEM adapter.
        return physics_.Solve(resolver_.Online(rhs));
    }

    LinearSystem& GetSystem() override { return resolver_.GetSystem(); }

    const Physics& physics() const { return physics_; }
    const Resolver& resolver() const { return resolver_; }
    Resolver& resolver() { return resolver_; }
    bool isSetup() const { return setup_; }
    bool isOffline() const { return offline_; }

private:
    Physics physics_;
    Resolver resolver_;
    bool setup_ = false;
    bool offline_ = false;
};

// Convenience aliases for the four combinations.
template <typename Self>
using FlowGalerkin = SubproblemSolver<FlowPhysics, GalerkinSolver, Self>;
template <typename Self>
using FlowMHM = SubproblemSolver<FlowPhysics, MultiscaleSolver, Self>;
template <typename Self>
using MechanicsGalerkin = SubproblemSolver<MechanicsPhysics, GalerkinSolver, Self>;
template <typename Self>
using MechanicsMHM = SubproblemSolver<MechanicsPhysics, MultiscaleSolver, Self>;

} // namespace msl_fss

#endif // MSL_FSS_SUBPROBLEMS_SUBPROBLEMSOLVER_HPP

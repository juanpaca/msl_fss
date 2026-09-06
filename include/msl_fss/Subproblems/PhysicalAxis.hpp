#ifndef MSL_FSS_SUBPROBLEMS_PHYSICALAXIS_HPP
#define MSL_FSS_SUBPROBLEMS_PHYSICALAXIS_HPP

#include "msl_fss/External/PhysicalFacade.hpp"
#include "msl_fss/Types/Config.hpp"
#include "msl_fss/Types/Types.hpp"

namespace msl_fss {

// Compile-time traits of the product: the regime flags (TRANSIENT /
// STATIONARY) mirror RegimeTrait<T> of msl_core; the arity (SCALAR /
// VECTORIAL) is local to the physical axis and selects the fachada used by
// the dummy implementation.
struct MFEMTraits {
    static constexpr bool TRANSIENT = true;
    static constexpr bool STATIONARY = false;
    static constexpr bool SCALAR = false;
    static constexpr bool VECTORIAL = true;
};

// Physical axis: Flow (pressure). Wraps the transient scalar facade
// (HeatMFEMProblem in the real MSL stack).
class FlowPhysics {
public:
    using FacadeType = external::PhysicalFacade<MFEMTraits::TRANSIENT, MFEMTraits::SCALAR>;
    static constexpr bool kTransient = true;
    static constexpr bool kVectorial = false;

    void Setup(const Config& config) {
        facade_.Setup(config);
        configured_ = true;
    }
    void AssembleSystem() { assembled_ = true; }
    Field Solve(const Field& rhs) const {
        return rhs; // dummy: identity solve
    }

    const FacadeType& facade() const { return facade_; }
    bool isConfigured() const { return configured_; }
    bool isAssembled() const { return assembled_; }

private:
    FacadeType facade_;
    bool configured_ = false;
    bool assembled_ = false;
};

// Physical axis: Mechanics (displacement). Wraps the stationary vectorial
// facade (ElasticityMFEMProblem in the real MSL stack).
class MechanicsPhysics {
public:
    using FacadeType = external::PhysicalFacade<MFEMTraits::STATIONARY, MFEMTraits::VECTORIAL>;
    static constexpr bool kTransient = false;
    static constexpr bool kVectorial = true;

    void Setup(const Config& config) {
        facade_.Setup(config);
        configured_ = true;
    }
    void AssembleSystem() { assembled_ = true; }
    Field Solve(const Field& rhs) const { return rhs; }

    const FacadeType& facade() const { return facade_; }
    bool isConfigured() const { return configured_; }
    bool isAssembled() const { return assembled_; }

private:
    FacadeType facade_;
    bool configured_ = false;
    bool assembled_ = false;
};

} // namespace msl_fss

#endif // MSL_FSS_SUBPROBLEMS_PHYSICALAXIS_HPP
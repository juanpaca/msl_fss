#ifndef MSL_FSS_EXTERNAL_PHYSICALFACADE_HPP
#define MSL_FSS_EXTERNAL_PHYSICALFACADE_HPP

#include "msl_fss/Types/Types.hpp"
#include "msl_fss/Types/Config.hpp"

#include <string>

namespace msl_fss {
namespace external {

// Stand-in for the real MSL-MFEM physical facade (e.g. HeatMFEMProblem for the
// flow and ElasticityMFEMProblem for the mechanics). In the real product these
// classes are reused integrally from msl_mfem; here they are minimal stand-ins
// that validate the coupling contract with dummy data.
//
// The traits indicate, at compile time, the regime (transient/stationary) and
// the field arity (scalar/vectorial) of each physical subproblem. The regime
// mirrors RegimeTrait<T> from msl_core; the arity is product-local.
template <bool TRANSIENT, bool VECTORIAL>
class PhysicalFacade {
public:
    static constexpr bool kTransient = TRANSIENT;
    static constexpr bool kVectorial = VECTORIAL;

    // Configures the facade from the simulation configuration.
    void Setup(const Config& config) {
        alpha_ = config.getDouble("alpha", 1.0);
        kdr_ = config.getDouble("Kdr", 1.0);
        size_ = static_cast<std::size_t>(config.getInt("ndof", 4));
        configured_ = true;
    }

    bool isConfigured() const { return configured_; }
    std::size_t size() const { return size_; }
    double alpha() const { return alpha_; }
    double Kdr() const { return kdr_; }

private:
    double alpha_ = 1.0;
    double kdr_ = 1.0;
    std::size_t size_ = 0;
    bool configured_ = false;
};

} // namespace external
} // namespace msl_fss

#endif // MSL_FSS_EXTERNAL_PHYSICALFACADE_HPP

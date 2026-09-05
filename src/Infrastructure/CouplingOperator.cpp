#include "msl_fss/Infrastructure/CouplingOperator.hpp"

namespace msl_fss {

Field CouplingOperator::ApplyPressureToMech(const Field& pressure,
                                            double scale) const {
    // Dummy: the pressure load in the mechanics is proportional to alpha.
    Field load(pressure.size());
    for (std::size_t i = 0; i < pressure.size(); ++i) {
        load[i] = alpha_ * scale * pressure[i];
    }
    return load;
}

Field CouplingOperator::ComputeVolumetricStress(const Field& u,
                                                const Field& p) const {
    // Dummy: sigmav = K_dr * div(u) - alpha * p. The divergence operator is
    // the identity in the dummy field (real: D_K matrix per macro-element).
    Field sig(u.size());
    for (std::size_t i = 0; i < u.size(); ++i) {
        sig[i] = Kdr_ * u[i] - alpha_ * p[i];
    }
    return sig;
}

} // namespace msl_fss
#include "msl_fss/Infrastructure/CouplingOperator.hpp"

namespace msl_fss {

Field CouplingOperator::ApplyPressureToMech(const Field& pressure,
                                            double scale) const {
    // load = alpha * D_K^T * P with the dummy D_K = I (the divergence matrix
    // of each macro-element in the real product, applied by integration by
    // parts).
    const DenseMatrix D = GetDivMatrix(0);
    const std::vector<double> dtp = D.mul(pressure.data());
    Field load(pressure.size());
    for (std::size_t i = 0; i < pressure.size(); ++i) {
        load[i] = alpha_ * scale * dtp[i];
    }
    return load;
}

DenseMatrix CouplingOperator::GetDivMatrix(std::size_t /*K*/) const {
    // Dummy divergence operator: identity per macro-element.
    DenseMatrix D(kLocalDofs_, kLocalDofs_);
    for (std::size_t i = 0; i < kLocalDofs_; ++i) D(i, i) = 1.0;
    return D;
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
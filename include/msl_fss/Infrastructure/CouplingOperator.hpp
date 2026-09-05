#ifndef MSL_FSS_INFRASTRUCTURE_COUPLINGOPERATOR_HPP
#define MSL_FSS_INFRASTRUCTURE_COUPLINGOPERATOR_HPP

#include "msl_fss/Types/Types.hpp"

#include <cstddef>

namespace msl_fss {

// Isolates the physical coupling between pressure and displacement (FR5),
// transversal to both orthogonal axes. The operator is strategy-agnostic:
//
//   Mechanics: load = alpha * D_K^T * P          (multiscale)
//              load = B^T * P                    (monoscale Galerkin)
//   Flow:      sigmav = K_dr * div(u) - alpha * P
//
// which avoids assembling a global coupling matrix B.
class CouplingOperator {
public:
    // alpha: Biot-Willis coefficient; Kdr: drained bulk modulus.
    CouplingOperator(double alpha, double Kdr) : alpha_(alpha), Kdr_(Kdr) {}

    // Injects the pressure into the mechanics by integration by parts.
    Field ApplyPressureToMech(const Field& pressure, double scale = 1.0) const;

    // Computes the (dummy) volumetric average stress per macro-element.
    Field ComputeVolumetricStress(const Field& u, const Field& p) const;

    double alpha() const { return alpha_; }
    double Kdr() const { return Kdr_; }

private:
    double alpha_;
    double Kdr_;
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_COUPLINGOPERATOR_HPP
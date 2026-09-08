#ifndef MSL_FSS_TYPES_HPP
#define MSL_FSS_TYPES_HPP

#include <algorithm>
#include <cstddef>
#include <vector>

namespace msl_fss {

// The two orthogonal axes combined by SubproblemSolver<Physics, Resolver, Self>.
//   FRAME: whether the subproblem is Flow (pressure, transient, scalar) or
//          Mechanics (displacement, stationary, vectorial).
//   RESOLVER: whether the subproblem is solved monoscale (Galerkin) or
//          multiscale (MHM).
//
// These enums are used for compile-time consistency checks only; the actual
// selection of a strategy is made at *compilation time* by the two main entry
// points (main_fss_galerkin / main_fss_mhm), not by runtime configuration.
enum class Frame { FLOW = 0, MECHANICS = 1 };

enum class Strategy { GALERKIN = 0, MHM = 1 };

// Possible outcomes of a Fixed-Stress Split iteration.
enum class FSSStatus {
    CONVERGED,   // relative error below tolerance within kMax iterations
    DIVERGED,    // residual increased beyond a safety bound
    MAX_ITER     // tolerance not reached within kMax iterations
};

// A dense dummy field of nodal/coefficient values. In the real product this is
// a MFEM GridFunction (pressure P or displacement U). Here it is a simple
// std::vector<double> so that the module structure can be validated without
// the MFEM dependency.
class Field {
public:
    Field() = default;
    explicit Field(std::size_t n) : data_(n, 0.0) {}

    std::size_t size() const { return data_.size(); }
    double& operator[](std::size_t i) { return data_[i]; }
    double operator[](std::size_t i) const { return data_[i]; }

    double& at(std::size_t i) { return data_.at(i); }
    double at(std::size_t i) const { return data_.at(i); }

    void resize(std::size_t n) { data_.resize(n, 0.0); }
    void fill(double v) { std::fill(data_.begin(), data_.end(), v); }

    const std::vector<double>& data() const { return data_; }
    std::vector<double>& data() { return data_; }

    // Difference used in the convergence test (\eqref{eq:convergence}).
    Field operator-(const Field& o) const;

private:
    std::vector<double> data_;
};

} // namespace msl_fss

#endif // MSL_FSS_TYPES_HPP

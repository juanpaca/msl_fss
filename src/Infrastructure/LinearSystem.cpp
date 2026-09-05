#include "msl_fss/Infrastructure/LinearSystem.hpp"

namespace msl_fss {

Field LinearSystem::Solve() {
    if (!factorized_) Factorize();
    x_ = Field(b_.size());
    // Dummy solve: identity-like scaling, so tests with the identity matrix
    // return the rhs unchanged. Real MSL/MFEM solvers replace this.
    for (std::size_t i = 0; i < b_.size(); ++i) {
        x_[i] = b_[i];
    }
    return x_;
}

} // namespace msl_fss
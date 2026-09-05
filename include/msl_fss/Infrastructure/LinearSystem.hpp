#ifndef MSL_FSS_INFRASTRUCTURE_LINEARSYSTEM_HPP
#define MSL_FSS_INFRASTRUCTURE_LINEARSYSTEM_HPP

#include "msl_fss/Infrastructure/Matrix.hpp"
#include "msl_fss/Types/Types.hpp"

namespace msl_fss {

// Encapsulates the linear solver used by each resolvedor.
//
// Galerkin path: a direct/iterative solver from the MFEM stack.
// Multiscale path: the distributed MUMPS solve of the skeleton problem.
//
// The dummy implementation stores the matrix/rhs and computes a trivial
// scaled solution, so that the call sequence (SetMatrix -> SetRHS -> Solve)
// used by both resolvedores can be validated without a real solver.
class LinearSystem {
public:
    LinearSystem() = default;

    void SetMatrix(const DenseMatrix& A) { A_ = A; }
    void SetRHS(const Field& b) { b_ = b; }
    void Factorize() { factorized_ = true; }

    // Dummy solve: x = A * b (identity-like scaling), only meaningful when the
    // matrix is identity. Real solvers replace this.
    Field Solve();

    const Field& GetSolution() const { return x_; }
    bool isFactorized() const { return factorized_; }

private:
    DenseMatrix A_;
    Field b_;
    Field x_;
    bool factorized_ = false;
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_LINEARSYSTEM_HPP

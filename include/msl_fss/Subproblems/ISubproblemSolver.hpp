#ifndef MSL_FSS_SUBPROBLEMS_ISUBPROBLEMSOLVER_HPP
#define MSL_FSS_SUBPROBLEMS_ISUBPROBLEMSOLVER_HPP

#include "msl_fss/Infrastructure/LinearSystem.hpp"
#include "msl_fss/Types/Types.hpp"

namespace msl_fss {

class SharedMHMHierarchy;

// Common contract of every subproblem, regardless of the combination of the
// physical axis (Flow/Mechanics) and the resolvedor axis
// (Galerkin/Multiscale). Introduced to allow unit tests and mock objects to
// exercise the FSS iteration independently of the concrete subproblems.
class ISubproblemSolver {
public:
    virtual ~ISubproblemSolver() = default;

    // Configures the subproblem with the (possibly shared) hierarchy.
    virtual void Setup(SharedMHMHierarchy* hierarchy) = 0;

    // One-time pre-computation: assembly and factorization of the operators.
    virtual void Offline() = 0;

    // Solves the subproblem with the updated right-hand side term.
    virtual Field Online(const Field& rhs) = 0;

    // Returns the underlying linear system.
    virtual LinearSystem& GetSystem() = 0;
};

} // namespace msl_fss

#endif // MSL_FSS_SUBPROBLEMS_ISUBPROBLEMSOLVER_HPP
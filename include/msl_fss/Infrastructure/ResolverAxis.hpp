#ifndef MSL_FSS_INFRASTRUCTURE_RESOLVERAXIS_HPP
#define MSL_FSS_INFRASTRUCTURE_RESOLVERAXIS_HPP

#include "msl_fss/Infrastructure/LinearSystem.hpp"
#include "msl_fss/Infrastructure/OfflineStore.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Subproblems/ISubproblemSolver.hpp"
#include "msl_fss/Types/Types.hpp"

namespace msl_fss {

// Resolvedor axis: monoscale Galerkin. The offline phase assembles a global
// finite-element space, the operator matrix, and factorizes it once; the
// online phase only solves by substitution with the updated right-hand side.
class GalerkinSolver : public ISubproblemSolver {
public:
    void Setup(SharedMHMHierarchy* /*hierarchy*/) override { setup_ = true; }
    void Offline() override {
        // Dummy: register a trivial (identity-like) system.
        DenseMatrix A(ndof_, ndof_);
        for (std::size_t i = 0; i < ndof_; ++i) A(i, i) = 1.0;
        system_.SetMatrix(A);
        system_.Factorize();
        offline_ = true;
    }
    Field Online(const Field& rhs) override {
        system_.SetRHS(rhs);
        return system_.Solve();
    }
    LinearSystem& GetSystem() override { return system_; }

    bool isSetup() const { return setup_; }
    bool isOffline() const { return offline_; }

private:
    LinearSystem system_;
    std::size_t ndof_ = 4;
    bool setup_ = false;
    bool offline_ = false;
};

// Resolvedor axis: multiscale MHM. The offline phase uses the shared hierarchy,
// walks over each macro-element assembling the local forms, and assembles the
// global skeleton problem; the online phase reconstructs the field from the
// stored local bases, reusing the factorizations.
class MultiscaleSolver : public ISubproblemSolver {
public:
    void Setup(SharedMHMHierarchy* hierarchy) override {
        hierarchy_ = hierarchy;
        setup_ = true;
    }
    void Offline() override {
        if (!hierarchy_) return;
        const auto& partition = hierarchy_->GetPartition();
        // Dummy: register one trivial contribution per macro-element and
        // assemble a global (identity) skeleton system.
        for (std::size_t K = partition.elementStart; K < partition.elementEnd; ++K) {
            DenseMatrix local(2, 2);
            local(0, 0) = 1.0;
            local(1, 1) = 1.0;
            store_.Store(K, local);
        }
        DenseMatrix G(ndof_, ndof_);
        for (std::size_t i = 0; i < ndof_; ++i) G(i, i) = 1.0;
        system_.SetMatrix(G);
        system_.Factorize();
        offline_ = true;
    }
    Field Online(const Field& rhs) override {
        system_.SetRHS(rhs);
        return system_.Solve();
    }
    LinearSystem& GetSystem() override { return system_; }

    const OfflineStore& store() const { return store_; }
    OfflineStore& store() { return store_; }
    const SharedMHMHierarchy* hierarchy() const { return hierarchy_; }
    bool isSetup() const { return setup_; }
    bool isOffline() const { return offline_; }

private:
    SharedMHMHierarchy* hierarchy_ = nullptr;
    LinearSystem system_;
    OfflineStore store_;
    std::size_t ndof_ = 4;
    bool setup_ = false;
    bool offline_ = false;
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_RESOLVERAXIS_HPP
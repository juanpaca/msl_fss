#ifndef MSL_FSS_INFRASTRUCTURE_SHAREDMHMHIERARCHY_HPP
#define MSL_FSS_INFRASTRUCTURE_SHAREDMHMHIERARCHY_HPP

#include "msl_fss/Infrastructure/PartitionInfo.hpp"
#include "msl_fss/Infrastructure/Matrix.hpp"
#include "msl_fss/Types/Config.hpp"

#include <vector>

namespace msl_fss {

// Encapsulates the shared geometric hierarchy: one single partition into
// macro-elements (splitToDisk), the edge mesh with the Lagrange multiplier
// space Lambda_H, and the fine sub-meshes, all shared between the flow and the
// mechanics sub-problems (FR2).
class SharedMHMHierarchy {
public:
    // Builds the hierarchy once. In the real product this performs the macro
    // partition and the local sub-mesh generation; in the dummy implementation
    // it just materializes the PartitionInfo and the per-element DOF counts.
    void Build(const Config& config);

    const PartitionInfo& GetPartition() const { return partition_; }
    PartitionInfo& GetPartition() { return partition_; }

    // Number of local DOFs of macro-element K (dummy).
    std::size_t GetSubMeshDofs(std::size_t K) const {
        return subMeshDofs_.at(K);
    }
    const std::vector<std::size_t>& GetSubMeshDofs() const { return subMeshDofs_; }

    std::size_t numSubMeshes() const { return subMeshDofs_.size(); }

private:
    PartitionInfo partition_;
    std::vector<std::size_t> subMeshDofs_;
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_SHAREDMHMHIERARCHY_HPP

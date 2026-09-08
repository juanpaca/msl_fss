#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"

namespace msl_fss {

void SharedMHMHierarchy::Build(const Config& config) {
    const int nElems = config.getInt("numelems", 4);
    partition_.numElements = static_cast<std::size_t>(nElems);
    partition_.elementStart = 0;
    partition_.elementEnd = static_cast<std::size_t>(nElems);
    partition_.rank = 0;
    partition_.mpiSize = 1;

    // Dummy sub-mesh DOF count per macro-element (real splitToDisk would
    // generate the fine sub-meshes here).
    const std::size_t localDofs = static_cast<std::size_t>(
        config.getInt("ndof_local", 2));
    edgeMesh_ = {0, partition_.numElements};
    subMeshes_.clear();
    subMeshes_.reserve(partition_.numElements);
    for (std::size_t K = 0; K < partition_.numElements; ++K) {
        subMeshes_.push_back({K, localDofs});
    }
}

std::vector<std::size_t> SharedMHMHierarchy::GetSubMeshDofs() const {
    std::vector<std::size_t> dofs;
    dofs.reserve(subMeshes_.size());
    for (const Mesh& mesh : subMeshes_) dofs.push_back(mesh.dofs);
    return dofs;
}

} // namespace msl_fss

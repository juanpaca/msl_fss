#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"

namespace msl_fss {

void SharedMHMHierarchy::Build(const Config& config) {
    const int nElems = config.getInt("numelems", 4);
    partition_.numElements = static_cast<std::size_t>(nElems);
    partition_.elementStart = 0;
    partition_.elementEnd = static_cast<std::size_t>(nElems);
    partition_.rank = 0;

    // Dummy sub-mesh DOF count per macro-element (real splitToDisk would
    // generate the fine sub-meshes here).
    subMeshDofs_.assign(partition_.numElements,
                        static_cast<std::size_t>(config.getInt("ndof_local", 2)));
}

} // namespace msl_fss
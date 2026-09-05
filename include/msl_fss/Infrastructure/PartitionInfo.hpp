#ifndef MSL_FSS_INFRASTRUCTURE_PARTITIONINFO_HPP
#define MSL_FSS_INFRASTRUCTURE_PARTITIONINFO_HPP

#include <cstddef>
#include <utility>

namespace msl_fss {

// Stores the slice of macro-elements assigned to the current MPI rank.
//
// In the real MSL product the macro-elements are distributed across ranks by
// slicing (FEMOO_slice_interval) and each rank owns a contiguous range
// [elementStart, elementEnd). This mirrors that structure with a plain struct
// so the distributed execution mapping can be validated in isolation.
struct PartitionInfo {
    std::size_t numElements = 0;   // total of macro-elements in the mesh
    std::size_t elementStart = 0;  // first macro-element owned by this rank
    std::size_t elementEnd = 0;    // one past the last owned macro-element
    int rank = 0;                  // current MPI rank

    std::size_t GetNumElements() const { return elementEnd - elementStart; }
    std::pair<std::size_t, std::size_t> GetRange() const {
        return {elementStart, elementEnd};
    }
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_PARTITIONINFO_HPP

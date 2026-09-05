#ifndef MSL_FSS_INFRASTRUCTURE_OFFLINESTORE_HPP
#define MSL_FSS_INFRASTRUCTURE_OFFLINESTORE_HPP

#include "msl_fss/Infrastructure/Matrix.hpp"
#include "msl_fss/Types/Types.hpp"

#include <map>
#include <vector>

namespace msl_fss {

// Central repository (Repository architectural style) that keeps the offline
// contributions and factorizations indexed by macro-element K. Both the flow
// and the mechanics resolvedores fetch their contributions from here during
// each FSS iteration, avoiding the re-assembly of local operators.
class OfflineStore {
public:
    using Key = std::size_t;

    void Store(Key key, const DenseMatrix& data) {
        contributions_[key] = data;
        ++size_;
    }
    bool Contains(Key key) const { return contributions_.count(key) > 0; }
    const DenseMatrix& Fetch(Key key) const { return contributions_.at(key); }

    void Clear() {
        contributions_.clear();
        size_ = 0;
    }
    std::size_t Size() const { return size_; }

private:
    std::map<Key, DenseMatrix> contributions_;
    std::size_t size_ = 0;
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_OFFLINESTORE_HPP

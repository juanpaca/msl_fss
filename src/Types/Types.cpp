#include "msl_fss/Types/Types.hpp"

#include <algorithm>
#include <stdexcept>

namespace msl_fss {

Field Field::operator-(const Field& o) const {
    if (data_.size() != o.data_.size()) {
        throw std::invalid_argument("Field::operator-: incompatible sizes");
    }
    Field diff(data_.size());
    for (std::size_t i = 0; i < data_.size(); ++i) {
        diff[i] = data_[i] - o.data_[i];
    }
    return diff;
}

} // namespace msl_fss
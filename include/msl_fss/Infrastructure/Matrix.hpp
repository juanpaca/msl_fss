#ifndef MSL_FSS_INFRASTRUCTURE_MATRIX_HPP
#define MSL_FSS_INFRASTRUCTURE_MATRIX_HPP

#include <cstddef>
#include <vector>

namespace msl_fss {

// Dense dummy matrix used to validate the CUDA-independent module structure.
// In the real product these are MFEM SparseMatrix (local/global) or the MUMPS
// distributed matrices of the skeleton problem. Here a small dense container
// is enough for the dummy implementations and the unit tests.
class DenseMatrix {
public:
    DenseMatrix() = default;
    DenseMatrix(std::size_t rows, std::size_t cols)
        : nrows_(rows), ncols_(cols), data_(rows * cols, 0.0) {}

    std::size_t rows() const { return nrows_; }
    std::size_t cols() const { return ncols_; }

    void resize(std::size_t rows, std::size_t cols) {
        nrows_ = rows;
        ncols_ = cols;
        data_.assign(rows * cols, 0.0);
    }

    double& operator()(std::size_t i, std::size_t j) { return data_[i * ncols_ + j]; }
    double operator()(std::size_t i, std::size_t j) const { return data_[i * ncols_ + j]; }

    // Basic matrix-vector product used by the coupling dummy implementations.
    std::vector<double> mul(const std::vector<double>& x) const {
        std::vector<double> y(nrows_, 0.0);
        for (std::size_t i = 0; i < nrows_; ++i) {
            double acc = 0.0;
            for (std::size_t j = 0; j < ncols_; ++j) {
                acc += (*this)(i, j) * x[j];
            }
            y[i] = acc;
        }
        return y;
    }

private:
    std::size_t nrows_ = 0;
    std::size_t ncols_ = 0;
    std::vector<double> data_;
};

} // namespace msl_fss

#endif // MSL_FSS_INFRASTRUCTURE_MATRIX_HPP

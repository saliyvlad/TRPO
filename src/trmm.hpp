#ifndef TRMM_HPP
#define TRMM_HPP

#include <cstddef>

enum class Side { Left, Right };
enum class Uplo { Upper, Lower };
enum class Trans { None, Transpose, ConjugateTranspose };
enum class Diag { NonUnit, Unit };

// Single precision
void trmm_s(Side side, Uplo uplo, Trans trans, Diag diag,
            size_t m, size_t n,
            float alpha,
            const float* A, size_t lda,
            float* B, size_t ldb);

// Double precision
void trmm_d(Side side, Uplo uplo, Trans trans, Diag diag,
            size_t m, size_t n,
            double alpha,
            const double* A, size_t lda,
            double* B, size_t ldb);

// Parallel versions (OpenMP)
void trmm_s_parallel(Side side, Uplo uplo, Trans trans, Diag diag,
                     size_t m, size_t n,
                     float alpha,
                     const float* A, size_t lda,
                     float* B, size_t ldb,
                     int num_threads);

void trmm_d_parallel(Side side, Uplo uplo, Trans trans, Diag diag,
                     size_t m, size_t n,
                     double alpha,
                     const double* A, size_t lda,
                     double* B, size_t ldb,
                     int num_threads);

#endif // TRMM_HPP
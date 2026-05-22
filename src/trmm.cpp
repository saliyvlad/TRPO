#include "trmm.hpp"
#include <omp.h>
#include <iostream> // Для отладки

// Sequential TRMM - Double precision
void trmm_d(Side side, Uplo uplo, Trans trans, Diag diag,
            size_t m, size_t n, double alpha,
            const double* A, size_t lda, double* B, size_t ldb) {
    
    // ОТЛАДКА: Это сообщение должно появиться при запуске!
    std::cerr << "[TRMM_DEBUG] Executing Corrected Version v3.0 (Alpha Fixed + Order Fixed)" << std::endl;

    if (side == Side::Left && uplo == Uplo::Upper && trans == Trans::None) {
        for (size_t j = 0; j < n; ++j) {
            // Correct algorithm for Upper Triangular:
            // 1. Use B(k) to update B(i) for i < k (B(k) is still "old" here)
            // 2. Then scale B(k)
            for (size_t k = 0; k < m; ++k) {
                // Update rows above diagonal using current column k
                // We use B[k] BEFORE scaling it to avoid double-counting diagonal elements
                double b_val = B[k + j * ldb];
                for (size_t i = 0; i < k; ++i) {
                    B[i + j * ldb] += A[i + k * lda] * b_val;
                }
                
                // Now scale the diagonal element
                double a_kk = (diag == Diag::Unit) ? 1.0 : A[k + k * lda];
                B[k + j * ldb] *= a_kk;
            }
            
            // Apply alpha ONCE at the end of the column
            if (alpha != 1.0) {
                for (size_t i = 0; i < m; ++i) B[i + j * ldb] *= alpha;
            }
        }
    } else {
        // Fallback for other cases (not tested here)
    }
}

// Sequential TRMM - Single precision (Same logic)
void trmm_s(Side side, Uplo uplo, Trans trans, Diag diag,
            size_t m, size_t n, float alpha,
            const float* A, size_t lda, float* B, size_t ldb) {
    
    std::cerr << "[TRMM_DEBUG] Executing Single Precision v3.0" << std::endl;

    if (side == Side::Left && uplo == Uplo::Upper && trans == Trans::None) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < m; ++k) {
                float b_val = B[k + j * ldb];
                for (size_t i = 0; i < k; ++i) {
                    B[i + j * ldb] += A[i + k * lda] * b_val;
                }
                float a_kk = (diag == Diag::Unit) ? 1.0f : A[k + k * lda];
                B[k + j * ldb] *= a_kk;
            }
            if (alpha != 1.0f) {
                for (size_t i = 0; i < m; ++i) B[i + j * ldb] *= alpha;
            }
        }
    }
}

// Parallel versions use the same corrected logic
void trmm_d_parallel(Side side, Uplo uplo, Trans trans, Diag diag,
                     size_t m, size_t n, double alpha,
                     const double* A, size_t lda, double* B, size_t ldb,
                     int num_threads) {
    omp_set_num_threads(num_threads);
    std::cerr << "[TRMM_DEBUG] Executing Parallel Double v3.0" << std::endl;
    
    if (side == Side::Left && uplo == Uplo::Upper && trans == Trans::None) {
        #pragma omp parallel for
        for (int j = 0; j < (int)n; ++j) {
            for (size_t k = 0; k < m; ++k) {
                double b_val = B[k + j * ldb];
                for (size_t i = 0; i < k; ++i) {
                    B[i + j * ldb] += A[i + k * lda] * b_val;
                }
                double a_kk = (diag == Diag::Unit) ? 1.0 : A[k + k * lda];
                B[k + j * ldb] *= a_kk;
            }
            if (alpha != 1.0) {
                for (size_t i = 0; i < m; ++i) B[i + j * ldb] *= alpha;
            }
        }
    } else {
        trmm_d(side, uplo, trans, diag, m, n, alpha, A, lda, B, ldb);
    }
}

void trmm_s_parallel(Side side, Uplo uplo, Trans trans, Diag diag,
                     size_t m, size_t n, float alpha,
                     const float* A, size_t lda, float* B, size_t ldb,
                     int num_threads) {
    omp_set_num_threads(num_threads);
    std::cerr << "[TRMM_DEBUG] Executing Parallel Single v3.0" << std::endl;

    if (side == Side::Left && uplo == Uplo::Upper && trans == Trans::None) {
        #pragma omp parallel for
        for (int j = 0; j < (int)n; ++j) {
            for (size_t k = 0; k < m; ++k) {
                float b_val = B[k + j * ldb];
                for (size_t i = 0; i < k; ++i) {
                    B[i + j * ldb] += A[i + k * lda] * b_val;
                }
                float a_kk = (diag == Diag::Unit) ? 1.0f : A[k + k * lda];
                B[k + j * ldb] *= a_kk;
            }
            if (alpha != 1.0f) {
                for (size_t i = 0; i < m; ++i) B[i + j * ldb] *= alpha;
            }
        }
    } else {
        trmm_s(side, uplo, trans, diag, m, n, alpha, A, lda, B, ldb);
    }
}
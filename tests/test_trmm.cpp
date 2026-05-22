#include "trmm.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <cblas.h>

bool compareMatrices(const std::vector<double>& a, const std::vector<double>& b, 
                    size_t m, size_t n, double tol = 1e-10) {
    for (size_t i = 0; i < m * n; ++i) {
        if (std::abs(a[i] - b[i]) > tol) {
            std::cout << "Mismatch at index " << i << ": " << a[i] 
                      << " vs " << b[i] << " (diff: " << std::abs(a[i] - b[i]) << ")\n";
            return false;
        }
    }
    return true;
}

int main() {
    size_t m = 100;
    size_t n = 100;
    size_t lda = m;
    size_t ldb = m;
    
    std::vector<double> A(m * m);
    std::vector<double> B(m * n);
    std::vector<double> B_ref(m * n);
    
    // Initialize upper triangular matrix A
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < m; ++j) {
            if (i <= j) {
                A[i + j * m] = 1.0 + 0.01 * (i + j);
            } else {
                A[i + j * m] = 0.0;
            }
        }
    }
    
    // Initialize matrix B
    for (size_t i = 0; i < m * n; ++i) {
        B[i] = 1.0 + 0.01 * i;
        B_ref[i] = B[i];
    }
    
    std::cout << "Testing TRMM implementation...\n";
    std::cout << "Matrix size: " << m << " x " << n << "\n\n";
    
    // Test our implementation
    std::cout << "Running custom TRMM...\n";
    trmm_d(Side::Left, Uplo::Upper, Trans::None, Diag::NonUnit,
           m, n, 1.0, A.data(), lda, B.data(), ldb);
    
    // Test OpenBLAS
    std::cout << "Running OpenBLAS TRMM...\n";
    cblas_dtrmm(CblasColMajor, CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit,
                m, n, 1.0, A.data(), lda, B_ref.data(), ldb);
    
    // Compare results
    std::cout << "\nComparing results...\n";
    if (compareMatrices(B, B_ref, m, n)) {
        std::cout << "✓ TEST PASSED: Results match OpenBLAS\n";
        return 0;
    } else {
        std::cout << "✗ TEST FAILED: Results differ from OpenBLAS\n";
        return 1;
    }
}
#include "trmm.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <iomanip>
#include <cblas.h>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

class TRMMBenchmark {
private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    
public:
    TRMMBenchmark() : rng(42), dist(-1.0, 1.0) {}
    
    void initMatrix(std::vector<float>& mat, size_t rows, size_t cols, bool triangular = false) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (triangular && i > j) {
                    mat[i + j * rows] = 0.0f;
                } else {
                    mat[i + j * rows] = static_cast<float>(dist(rng));
                }
            }
        }
    }
    
    void initMatrix(std::vector<double>& mat, size_t rows, size_t cols, bool triangular = false) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (triangular && i > j) {
                    mat[i + j * rows] = 0.0;
                } else {
                    mat[i + j * rows] = dist(rng);
                }
            }
        }
    }
    
    template<typename T>
    double runBenchmark(void (*trmm_func)(Side, Uplo, Trans, Diag, size_t, size_t, T, const T*, size_t, T*, size_t, int),
                       size_t m, size_t n, int num_threads, int iterations = 10) {
        std::vector<T> A(m * m);
        std::vector<T> B(m * n);
        
        initMatrix(A, m, m, true);
        initMatrix(B, m, n);
        
        // Warm-up
        trmm_func(Side::Left, Uplo::Upper, Trans::None, Diag::NonUnit,
                  m, n, T(1.0), A.data(), m, B.data(), m, num_threads);
        
        // Benchmark
#ifdef _WIN32
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
#else
        auto start = std::chrono::high_resolution_clock::now();
#endif
        
        for (int i = 0; i < iterations; ++i) {
            initMatrix(B, m, n);
            trmm_func(Side::Left, Uplo::Upper, Trans::None, Diag::NonUnit,
                      m, n, T(1.0), A.data(), m, B.data(), m, num_threads);
        }
        
#ifdef _WIN32
        QueryPerformanceCounter(&end);
        double elapsed = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
#else
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
#endif
        
        return elapsed / iterations;
    }
    
    template<typename T>
    double runOpenBLASBenchmark(size_t m, size_t n, int iterations = 10) {
        std::vector<T> A(m * m);
        std::vector<T> B(m * n);
        
        initMatrix(A, m, m, true);
        initMatrix(B, m, n);
        
        CBLAS_SIDE side = CblasLeft;
        CBLAS_UPLO uplo = CblasUpper;
        CBLAS_TRANSPOSE trans = CblasNoTrans;
        CBLAS_DIAG diag = CblasNonUnit;
        
        // Warm-up
        if constexpr (std::is_same<T, float>::value) {
            cblas_strmm(CblasColMajor, side, uplo, trans, diag, m, n, 1.0f, 
                       A.data(), m, B.data(), m);
        } else {
            cblas_dtrmm(CblasColMajor, side, uplo, trans, diag, m, n, 1.0, 
                       A.data(), m, B.data(), m);
        }
        
        // Benchmark
#ifdef _WIN32
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
#else
        auto start = std::chrono::high_resolution_clock::now();
#endif
        
        for (int i = 0; i < iterations; ++i) {
            initMatrix(B, m, n);
            if constexpr (std::is_same<T, float>::value) {
                cblas_strmm(CblasColMajor, side, uplo, trans, diag, m, n, 1.0f, 
                           A.data(), m, B.data(), m);
            } else {
                cblas_dtrmm(CblasColMajor, side, uplo, trans, diag, m, n, 1.0, 
                           A.data(), m, B.data(), m);
            }
        }
        
#ifdef _WIN32
        QueryPerformanceCounter(&end);
        double elapsed = static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
#else
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
#endif
        
        return elapsed / iterations;
    }
};

int main() {
    TRMMBenchmark benchmark;
    
    // Matrix size - adjust to get at least 1 minute execution
    size_t m = 2000;
    size_t n = 2000;
    int iterations = 10;
    
    std::cout << "TRMM Performance Benchmark\n";
    std::cout << "==========================\n\n";
    std::cout << "Matrix size: " << m << " x " << n << "\n";
    std::cout << "Iterations: " << iterations << "\n\n";
    
    std::vector<int> threads = {1, 2, 4, 8, 16};
    
    std::cout << std::fixed << std::setprecision(6);
    
    // Single precision
    std::cout << "SINGLE PRECISION (float)\n";
    std::cout << "------------------------\n";
    std::cout << std::setw(10) << "Threads" 
              << std::setw(15) << "Time (s)" 
              << std::setw(15) << "OpenBLAS (s)" 
              << std::setw(15) << "Performance %" << "\n";
    std::cout << std::string(55, '-') << "\n";
    
    for (int t : threads) {
        double my_time = benchmark.runBenchmark<float>(trmm_s_parallel, m, n, t, iterations);
        double openblas_time = benchmark.runOpenBLASBenchmark<float>(m, n, iterations);
        double perf = (openblas_time / my_time) * 100.0;
        
        std::cout << std::setw(10) << t 
                  << std::setw(15) << my_time 
                  << std::setw(15) << openblas_time 
                  << std::setw(14) << perf << "%\n";
    }
    
    std::cout << "\n";
    
    // Double precision
    std::cout << "DOUBLE PRECISION (double)\n";
    std::cout << "-------------------------\n";
    std::cout << std::setw(10) << "Threads" 
              << std::setw(15) << "Time (s)" 
              << std::setw(15) << "OpenBLAS (s)" 
              << std::setw(15) << "Performance %" << "\n";
    std::cout << std::string(55, '-') << "\n";
    
    for (int t : threads) {
        double my_time = benchmark.runBenchmark<double>(trmm_d_parallel, m, n, t, iterations);
        double openblas_time = benchmark.runOpenBLASBenchmark<double>(m, n, iterations);
        double perf = (openblas_time / my_time) * 100.0;
        
        std::cout << std::setw(10) << t 
                  << std::setw(15) << my_time 
                  << std::setw(15) << openblas_time 
                  << std::setw(14) << perf << "%\n";
    }
    
    std::cout << "\nBenchmark completed successfully!\n";
    
    return 0;
}
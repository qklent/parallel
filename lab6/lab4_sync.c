#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double sequential_calc(const double* A, const double* B, int N) {
    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        double C = A[i] / (B[i] + 1);
        sum += C;
    }
    return sum;
}

double parallel_calc_barrier(const double* A, const double* B, int N) {
    double sum = 0.0;
    #pragma omp parallel shared(sum)
    {
        double local_sum = 0.0;
        #pragma omp for nowait
        for (int i = 0; i < N; ++i) {
            double C = A[i] / (B[i] + 1);
            local_sum += C;
        }
        
        #pragma omp barrier
        #pragma omp critical
        sum += local_sum;
    }
    return sum;
}

double parallel_calc_barrier_chunks(const double* A, const double* B, int N) {
    double sum = 0.0;
    const int chunk_size = 1000; // Можно настроить размер чанка

    #pragma omp parallel shared(sum)
    {
        double local_sum = 0.0;
        #pragma omp for schedule(static, chunk_size) nowait
        for (int i = 0; i < N; ++i) {
            double C = A[i] / (B[i] + 1);
            local_sum += C;
        }

        #pragma omp barrier
        #pragma omp atomic
        sum += local_sum;
    }
    return sum;
}

void fill_random(double* vec, int N) {
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        vec[i] = (double)rand() / RAND_MAX * 99.0 + 1.0;
    }
}

double measure_time(double (*func)(const double*, const double*, int), const double* A, const double* B, int N) {
    double start = omp_get_wtime();
    double result = func(A, B, N);
    double end = omp_get_wtime();
    return (end - start) * 1000.0;
}

int main() {
    int sizes[] = {1000, 10000, 100000, 1000000, 10000000, 100000000, 200000000, 500000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; ++i) {
        int N = sizes[i];
        double *A = (double*)malloc(N * sizeof(double));
        double *B = (double*)malloc(N * sizeof(double));

        fill_random(A, N);
        fill_random(B, N);

        double seq_time = measure_time(sequential_calc, A, B, N);
        double barrier_time = measure_time(parallel_calc_barrier, A, B, N);
        double barrier_chunks_time = measure_time(parallel_calc_barrier_chunks, A, B, N);

        printf("N = %d:\n", N);
        printf("  Sequential time: %.2f ms\n", seq_time);
        printf("  Parallel time (barrier): %.2f ms\n", barrier_time);
        printf("  Parallel time (barrier chunks): %.2f ms\n", barrier_chunks_time);
        printf("  Speedup (barrier): %.2fx\n", seq_time / barrier_time);
        printf("  Speedup (barrier chunks): %.2fx\n\n", seq_time / barrier_chunks_time);

        free(A);
        free(B);
    }

    return 0;
}
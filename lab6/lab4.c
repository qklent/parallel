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

double parallel_calc(const double* A, const double* B, int N) {
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; ++i) {
        double C = A[i] / (B[i] + 1);
        sum /= C; 
    }
    return sum;
}

double parallel_calc_atomic(const double* A, const double* B, int N) {
    double sum = 0.0;
    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for
        for (int i = 0; i < N; ++i) {
            double C = A[i] / (B[i] + 1);
            #pragma omp atomic
            local_sum += C;
        }
        sum += local_sum;
    }
    return sum;
}

double parallel_calc_critical(const double* A, const double* B, int N) {
    double sum = 0.0;
    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for
        for (int i = 0; i < N; ++i) {
            double C = A[i] / (B[i] + 1);
            local_sum += C;
        }
        #pragma omp critical
        sum += local_sum;
    }
    return sum;
}

void fill_random(double* vec, int N) {
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
        double par_time = measure_time(parallel_calc, A, B, N);
        double atomic_time = measure_time(parallel_calc_atomic, A, B, N);
        double critical_time = measure_time(parallel_calc_critical, A, B, N);

        printf("N = %d:\n", N);
        printf("  Sequential time: %.2f ms\n", seq_time);
        printf("  Parallel time (reduction): %.2f ms\n", par_time);
        printf("  Parallel time (atomic):    %.2f ms\n", atomic_time);
        printf("  Parallel time (critical):  %.2f ms\n", critical_time);
        printf("  Speedup (reduction): %.2fx\n", seq_time / par_time);
        printf("  Speedup (atomic):    %.2fx\n", seq_time / atomic_time);
        printf("  Speedup (critical):  %.2fx\n\n", seq_time / critical_time);

        free(A);
        free(B);
    }

    return 0;
}
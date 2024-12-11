#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double parallel_calc(const double* A, const double* B, int N) {
    double sum = 0.0;
    double* local_sums;
    int num_threads;

    #pragma omp parallel
    {
        #pragma omp single
        {
            num_threads = omp_get_num_threads();
            local_sums = (double*)calloc(num_threads, sizeof(double));
        }
        
        int thread_id = omp_get_thread_num();
        double local_sum = 0.0;

        #pragma omp for nowait
        for (int i = 0; i < N; ++i) {
            double C = A[i] / (B[i] + 1);
            local_sum += C;
        }
        
        local_sums[thread_id] = local_sum;
        
        #pragma omp barrier 
        
        #pragma omp single
        {
            for (int i = 0; i < num_threads; i++) {
                sum += local_sums[i];
            }
            free(local_sums);
        }
    }
    return sum;
}


double sequential_calc(const double* A, const double* B, int N) {
    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        double C = A[i] / (B[i] + 1);
        sum += C;
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
    int sizes[] = {1000, 10000, 100000, 1000000, 10000000, 100000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; ++i) {
        int N = sizes[i];
        double *A = (double*)malloc(N * sizeof(double));
        double *B = (double*)malloc(N * sizeof(double));

        fill_random(A, N);
        fill_random(B, N);

        double seq_time = measure_time(sequential_calc, A, B, N);
        double par_time = measure_time(parallel_calc, A, B, N);

        printf("N = %d:\n", N);
        printf("  Sequential time: %.2f ms\n", seq_time);
        printf("  Parallel time:   %.2f ms\n", par_time);
        printf("  Speedup:         %.2fx\n\n", seq_time / par_time);

        free(A);
        free(B);
    }

    return 0;
}
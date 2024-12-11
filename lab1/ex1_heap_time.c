#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define M 2000
#define N 2000
#define MAX_NUM 1000000 

int count_sevens(int num) {
    int count = 0;
    while (num > 0) {
        if (num % 10 == 7) {
            count++;
        }
        num /= 10;
    }
    return count;
}


double measure_time(void (*func)(int** matrix, int* results), int** matrix, int* results) {
    double start = omp_get_wtime();
    func(matrix, results);
    double end = omp_get_wtime();
    return (end - start) * 1000.0;
}

void count_sevens_for_array(int** matrix, int* results){
    #pragma omp parallel for
    for (int i = 0; i < M; i++) {
        int local_count = 0;
        for (int j = 0; j < N; j++) {
            for (int k = j + 1; k < N; k++) {
                int sum = matrix[i][j] + matrix[i][k];
                local_count += count_sevens(sum);
            }
        }
        results[i] = local_count;
    }
}


int main() {
    int **matrix;
    int *results;
    
    matrix = (int **)malloc(M * sizeof(int *));
    if (matrix == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для matrix\n");
        return 1;
    }
    for (int i = 0; i < M; i++) {
        matrix[i] = (int *)malloc(N * sizeof(int));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Ошибка выделения памяти для matrix[%d]\n", i);
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return 1;
        }
    }
    
    results = (int *)malloc(M * sizeof(int));
    if (results == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для results\n");
        for (int i = 0; i < M; i++) {
            free(matrix[i]);
        }
        free(matrix);
        return 1;
    }
    

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % MAX_NUM + 1;
        }
    }
    
    printf("starting parallel computing\n");

    double time = measure_time(count_sevens_for_array, matrix, results);

    printf("Parallel computing took %fs", time / 1000);
    
    // for (int i = 0; i < M; i++) {
    //     printf("Строка %d: %d семерок\n", i + 1, results[i]);
    // }
    
    for (int i = 0; i < M; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(results);
    
    return 0;
}
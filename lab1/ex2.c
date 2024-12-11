
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define M 10000 
#define N 1000
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

int main() {
    int matrix[M][N];
    int results[M] = {0};
    
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % MAX_NUM + 1;
        }
    }
    

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
    
    for (int i = 0; i < M; i++) {
        printf("Строка %d: %d семерок\n", i + 1, results[i]);
    }
    
    return 0;
}

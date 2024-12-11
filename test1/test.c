#include "omp.h"
#include "stdio.h"

void work(int i){
    int thread_num = omp_get_thread_num();
    printf("thread number %d for iteration %d\n", thread_num, i);
};

int main() {
    int n;
    n = 10;
    omp_set_num_threads(2);
    #pragma omp parallel num_threads(4) if (n > 5)
    {
        #pragma omp for
            for (int i = 0; i < n; i++) {
                work(i);
            }
    }
}
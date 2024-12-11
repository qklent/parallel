#include <omp.h> 
#include <stdio.h>

// testing race condition
int main() {
    int i = 0;
    
    omp_set_num_threads(4);
    
    #pragma omp parallel shared(i)
    {
        while (i < 20)
        {
            printf("Thread %d: %d Hello World !\n", omp_get_thread_num(), i);
            i++; 
        }
    }
    
    return 0;
}
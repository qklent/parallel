#include "omp.h"

int main(){
    int numt = 0;
    #pragma omp parallel
    {
        #pragma omp master
        {
            #pragma omp critical
            {
                numt = omp_get_num_threads();
            }
            #pragma omp barrier
        }
    }
}
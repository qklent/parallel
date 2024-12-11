#define N 100
#include "omp.h"

void work(int i);

int main(){
    #pragma omp parallel
    {
        omp_set_schedule(omp_sched_dynamic);
        #pragma omp for schedule(static)
            for (int i = 0; i < N; i++) work(i);
    }
}
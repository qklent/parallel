#include <omp.h>
#include <stdio.h>

const int NMAX = 11;
const int LIMIT = 10;


void main()
{
    int i, j;
    float sum;
    float a[NMAX][NMAX];

    for (i = 0; i < NMAX; i++)
        for (j = 0; j < NMAX; j++)
            a[i][j] = i + j;

#pragma omp parallel shared(a) if (NMAX > LIMIT)
    {
#pragma omp for private(i, j, sum)
        for (i = 0; i < NMAX; i++)
        {
            sum = 0;
            for (j = 0; j < NMAX; j++)
                sum += a[i][j];
            printf("Сумма элементов строки %d равна %f\n", i, sum);
        }
    } /* Завершение параллельного фрагмента */
}
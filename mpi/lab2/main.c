#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define STOP_VALUE -1

int main(int argc, char** argv) {
    int rank, size;
    int counter = 0;
    int value;
    int all_finished = 0;
    int *values;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("Требуется как минимум 2 процесса\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        values = (int *)malloc(size * sizeof(int));
    }

    for (int iteration = 0; !all_finished && iteration < 3; iteration++) {
        value = 0;
        if (rank != 0) {
            value = (iteration < 3) ? rank : (rank == 1 ? STOP_VALUE : rank);
        }

        MPI_Gather(&value, 1, MPI_INT, values, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            all_finished = 1; 
            for (int i = 0; i < size; i++) {
                if (values[i] == STOP_VALUE) {
                    all_finished = 0;
                    break;
                }
            }

            if (all_finished) {
                printf("Процесс-счетчик завершает работу. Финальное значение: %d\n", counter);
            } else {
                int sum = 0;
                for (int i = 0; i < size; i++) {
                    sum += values[i];
                }
                counter++;
                printf("Получено значение: %d. Новое значение счетчика: %d\n", sum, counter);
            }
        }

        MPI_Bcast(&all_finished, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        free(values);
    }

    MPI_Finalize();
    return 0;
}

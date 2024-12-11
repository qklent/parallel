#include <stdio.h>
#include <mpi.h>

#define STOP_VALUE -1

int main(int argc, char** argv) {
    int rank, size;
    int counter = 0; 
    int received_value;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("Требуется как минимум 2 процесса\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) { 
        while (1 ) {
            MPI_Recv(&received_value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            if (received_value == STOP_VALUE) {
                printf("Процесс-счетчик завершает работу. Финальное значение: %d\n", counter);
                break;
            } else {
                counter++;
                printf("Получено значение: %d. Новое значение счетчика: %d\n", received_value, counter);
            }
        }
    } else {  
        for (int i = 0; i < 3; i++) {
            int value = rank;  
            MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        if (rank == 1) { 
            int stop = STOP_VALUE;
            MPI_Send(&stop, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
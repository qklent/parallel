#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

double determinant(vector<vector<double>>& matrix, int size) {
    if (size == 1) return matrix[0][0];
    if (size == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

    double det = 0;
    int sign = 1;
    
    for (int i = 0; i < size; i++) {
        vector<vector<double>> submatrix(size - 1, vector<double>(size - 1));
        
        for (int j = 1; j < size; j++) {
            for (int k = 0; k < size; k++) {
                if (k < i) submatrix[j-1][k] = matrix[j][k];
                else if (k > i) submatrix[j-1][k-1] = matrix[j][k];
            }
        }
        
        det += sign * matrix[0][i] * determinant(submatrix, size - 1);
        sign = -sign;
    }
    
    return det;
}

vector<vector<double>> createMatrixForXi(vector<vector<double>>& original, vector<double>& b, int column, int size) {
    vector<vector<double>> result = original;
    for (int i = 0; i < size; i++) {
        result[i][column] = b[i];
    }
    return result;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Создаем две группы процессов
    MPI_Group world_group, workers_group;
    MPI_Comm workers_comm;
    
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    
    // Исключаем процесс с рангом 0 (координатор)
    int ranks[1] = {0};
    MPI_Group_excl(world_group, 1, ranks, &workers_group);
    MPI_Comm_create(MPI_COMM_WORLD, workers_group, &workers_comm);

    const int N = 3; // Размер системы уравнений

    if (rank == 0) { // Координатор
        // Инициализация матрицы коэффициентов и вектора свободных членов
        vector<vector<double>> A = {
            {2, -1, 5},
            {3, 2, 1},
            {1, -3, 2}
        };
        vector<double> b = {11, 8, 3};

        // Вычисление основного определителя
        double mainDet = determinant(A, N);
        
        if (fabs(mainDet) < 1e-10) {
            cout << "Система не имеет единственного решения (определитель равен 0)" << endl;
        } else {
            vector<double> result(N);
            
            // Распределение работы между рабочими процессами
            for (int i = 0; i < N; i++) {
                vector<vector<double>> Ai = createMatrixForXi(A, b, i, N);
                double detAi = determinant(Ai, N);
                result[i] = detAi / mainDet;
            }

            // Вывод результата
            cout << "Решение системы:" << endl;
            for (int i = 0; i < N; i++) {
                cout << "x" << i + 1 << " = " << result[i] << endl;
            }
        }
    } else if (workers_comm != MPI_COMM_NULL) { // Рабочие процессы
        // Здесь может быть реализована параллельная обработка
        // частей матрицы для оптимизации вычислений
    }

    if (workers_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&workers_comm);
    }
    MPI_Group_free(&world_group);
    MPI_Group_free(&workers_group);
    
    MPI_Finalize();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

// Define o tamanho da matriz fixa
#define N 4

// Matriz e vetor fixos para teste
double matrix[N][N] = {
    {2, 1, -1, -3},
    {-1, 3, 2, 1},
    {3, -2, 1, 2},
    {1, 2, -1, -1}
};

double b[N] = {8, 1, -4, -2};

void print_matrix(double matrix[N][N], double b[N], int n) {
    printf("Matriz A|b:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6.2f ", matrix[i][j]);
        }
        printf("| %6.2f\n", b[i]);
    }
    printf("\n");
}

void print_solution(double x[N], int n) {
    printf("Solução do sistema (x):\n");
    for (int i = 0; i < n; i++) {
        printf("x[%d] = %6.2f\n", i, x[i]);
    }
    printf("\n");
}

void gaussian_elimination(double matrix[N][N], double b[N], double x[N], int n, int rank, int size) {
    for (int k = 0; k < n; k++) {
        int pivot_owner = k % size;

        // Processo responsável pelo pivô
        if (rank == pivot_owner) {
            printf("Processo %d: Normalizando linha pivô %d\n", rank, k);
            for (int j = k + 1; j < n; j++) {
                matrix[k][j] /= matrix[k][k];
            }
            b[k] /= matrix[k][k];
            matrix[k][k] = 1.0;
        }

        // Broadcast da linha pivô
        MPI_Bcast(&matrix[k][0], n, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);
        MPI_Bcast(&b[k], 1, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);

        // Atualização das linhas abaixo do pivô
        for (int i = rank; i < n; i += size) {
            if (i > k) {
                double factor = matrix[i][k];
                for (int j = k + 1; j < n; j++) {
                    matrix[i][j] -= factor * matrix[k][j];
                }
                b[i] -= factor * b[k];
                matrix[i][k] = 0.0;
                printf("Processo %d: Atualizou linha %d\n", rank, i);
            }
        }

        // Impressão parcial da matriz
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            printf("Matriz após etapa %d:\n", k);
            print_matrix(matrix, b, n);
        }
    }

    // Substituição reversa
    for (int i = n - 1; i >= 0; i--) {
        if (rank == i % size) {
            x[i] = b[i];
            for (int j = i + 1; j < n; j++) {
                x[i] -= matrix[i][j] * x[j];
            }
        }
        MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
    }

    // Impressão da solução parcial
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Solução parcial após substituição reversa:\n");
        print_solution(x, n);
    }
}

int main(int argc, char **argv) {
    int rank, size;
    double x[N] = {0}; // Inicializa o vetor solução com zeros

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Impressão inicial
    if (rank == 0) {
        printf("Matriz inicial (A|b):\n");
        print_matrix(matrix, b, N);
    }

    // Broadcast dos dados iniciais
    MPI_Bcast(matrix, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Executa eliminação de Gauss
    gaussian_elimination(matrix, b, x, N, rank, size);

    // Solução final
    if (rank == 0) {
        printf("\nSolução final (x):\n");
        print_solution(x, N);
    }

    MPI_Finalize();
    return 0;
}
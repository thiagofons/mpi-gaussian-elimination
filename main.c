#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

// Define os tamanhos das matrizes
#define SMALL_SIZE 4
#define MEDIUM_SIZE 100
#define LARGE_SIZE 500

// Função para gerar uma matriz aleatória e vetor de constantes
void generate_matrix(double *matrix, double *b, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = rand() % 10 + 1; // Valores entre 1 e 10
        }
        b[i] = rand() % 10 + 1;
    }
}

// Função para imprimir a matriz
void print_matrix(double *matrix, double *b, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6.2f ", matrix[i * n + j]);
        }
        printf("| %6.2f\n", b[i]);
    }
}

// Método de eliminação de Gauss com paralelismo MPI
void gaussian_elimination(double *matrix, double *b, double *x, int n, int rank, int size) {
    for (int k = 0; k < n; k++) {
        // Determina o processo que gerencia a linha pivô
        int pivot_owner = k % size;

        // Broadcast da linha pivô
        if (rank == pivot_owner) {
            for (int j = k + 1; j < n; j++) {
                matrix[k * n + j] /= matrix[k * n + k];
            }
            b[k] /= matrix[k * n + k];
            matrix[k * n + k] = 1.0; // Normaliza a linha pivô
        }
        MPI_Bcast(&matrix[k * n], n - k, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);
        MPI_Bcast(&b[k], 1, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);

        // Atualiza as outras linhas
        for (int i = rank; i < n; i += size) {
            if (i > k) {
                double factor = matrix[i * n + k];
                for (int j = k + 1; j < n; j++) {
                    matrix[i * n + j] -= factor * matrix[k * n + j];
                }
                b[i] -= factor * b[k];
                matrix[i * n + k] = 0.0;
            }
        }
    }

    // Solução por substituição reversa (back substitution)
    for (int i = n - 1; i >= 0; i--) {
        if (rank == i % size) {
            x[i] = b[i];
            for (int j = i + 1; j < n; j++) {
                x[i] -= matrix[i * n + j] * x[j];
            }
        }
        MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv) {
    int rank, size, n;
    double *matrix, *b, *x;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Define o tamanho da matriz com base nos argumentos
    if (argc != 2) {
        if (rank == 0) printf("Uso: %s <small|medium|large>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    if (strcmp(argv[1], "small") == 0) {
        n = SMALL_SIZE;
    } else if (strcmp(argv[1], "medium") == 0) {
        n = MEDIUM_SIZE;
    } else if (strcmp(argv[1], "large") == 0) {
        n = LARGE_SIZE;
    } else {
        if (rank == 0) printf("Tamanho inválido. Use small, medium ou large.\n");
        MPI_Finalize();
        return 1;
    }

    // Aloca memória
    matrix = (double *)malloc(n * n * sizeof(double));
    b = (double *)malloc(n * sizeof(double));
    x = (double *)malloc(n * sizeof(double));

    // Gera matriz e vetor b
    if (rank == 0) {
        generate_matrix(matrix, b, n);
        printf("Matriz inicial (A|b):\n");
        print_matrix(matrix, b, n);
    }

    // Broadcast da matriz e vetor b
    MPI_Bcast(matrix, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Executa eliminação de Gauss
    gaussian_elimination(matrix, b, x, n, rank, size);

    // Resultado
    if (rank == 0) {
        printf("\nSolução do sistema (x):\n");
        for (int i = 0; i < n; i++) {
            printf("x[%d] = %6.2f\n", i, x[i]);
        }
    }

    // Libera memória
    free(matrix);
    free(b);
    free(x);

    MPI_Finalize();
    return 0;
}

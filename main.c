#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define N 500 // Tamanho fixo da matriz

// Função para carregar a matriz e o vetor de um arquivo
void load_matrix_from_file(const char *filename, double matrix[N][N], double b[N], int n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo para leitura");
        exit(EXIT_FAILURE);
    }

    char line[N*6];
    int row = 0;

    while (fgets(line, sizeof(line), file) && row < n) {
        char *token = strtok(line, ",;");
        for (int col = 0; col < n; col++) {
            if (token) {
                matrix[row][col] = atof(token);
                token = strtok(NULL, ",;");
            }
        }
        if (token) {
            b[row] = atof(token);
        }
        row++;
    }

    fclose(file);
    printf("Matriz carregada do arquivo: %s\n", filename);
}

// Função para imprimir a matriz
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

// Função para imprimir a solução
void print_solution(double x[N], int n) {
    printf("Solução do sistema (x):\n");
    for (int i = 0; i < n; i++) {
        printf("x[%d] = %6.2f\n", i, x[i]);
    }
    printf("\n");
}

// Método de eliminação de Gauss
void gaussian_elimination(double matrix[N][N], double b[N], double x[N], int n, int rank, int size) {
    for (int k = 0; k < n; k++) {
        int pivot_owner = k % size;

        if (rank == pivot_owner) {
           /*  printf("Processo %d: Normalizando linha pivô %d\n", rank, k); */
            for (int j = k + 1; j < n; j++) {
                matrix[k][j] /= matrix[k][k];
            }
            b[k] /= matrix[k][k];
            matrix[k][k] = 1.0;
        }

        MPI_Bcast(&matrix[k][0], n, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);
        MPI_Bcast(&b[k], 1, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);

        for (int i = rank; i < n; i += size) {
            if (i > k) {
                double factor = matrix[i][k];
                for (int j = k + 1; j < n; j++) {
                    matrix[i][j] -= factor * matrix[k][j];
                }
                b[i] -= factor * b[k];
                matrix[i][k] = 0.0;
                //printf("Processo %d: Atualizou linha %d\n", rank, i);
            }
        }

        /*MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            printf("Matriz após etapa %d:\n", k);
            print_matrix(matrix, b, n);
        }*/
    }

    for (int i = n - 1; i >= 0; i--) {
        if (rank == i % size) {
            x[i] = b[i];
            for (int j = i + 1; j < n; j++) {
                x[i] -= matrix[i][j] * x[j];
            }
        }
        MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
    }

    /* MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Solução parcial após substituição reversa:\n");
        print_solution(x, n);
    } */
}

int main(int argc, char **argv) {
    int rank, size;
    double matrix[N][N];
    double b[N];
    double x[N] = {0};
    double t_inicial, t_final;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Processo %d de %d iniciado\n", rank, size);

    if (rank == 0) {
        // Carrega a matriz do arquivo
        const char *filename = "matrix.txt";
        load_matrix_from_file(filename, matrix, b, N);
    }

    t_inicial = MPI_Wtime();

    // Broadcast dos dados
    MPI_Bcast(matrix, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Impressão inicial
    if (rank == 0) {
        printf("Matriz inicial carregada (A|b):\n");
        //print_matrix(matrix, b, N);
    }

    // Executa eliminação de Gauss
    gaussian_elimination(matrix, b, x, N, rank, size);

    t_final = MPI_Wtime();

    if (rank == 0) {
        printf("\nSolução final (x):\n");
        print_solution(x, N);
        printf("Tempo de execução: %.6f segundos\n", t_final - t_inicial);
    }

    MPI_Finalize();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define N 3 // Tamanho fixo da matriz

// Função para carregar a matriz e o vetor de um arquivo
void load_matrix_from_file(const char *filename, double *matrix, double *b, int n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo para leitura");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;  // Ponteiro para armazenar a linha
    size_t len = 0;     // Tamanho do buffer da linha
    int row = 0;        // Índice da linha

    while (getline(&line, &len, file) != -1 && row < n) {
        char *token = strtok(line, ",;"); // Primeiro token da linha
        for (int col = 0; col < n; col++) {
            if (token) {
                matrix[row * n + col] = atof(token); // Armazena no vetor unidimensional
                token = strtok(NULL, ",;");
            }
        }
        if (token) {
            b[row] = atof(token); // Último elemento da linha vai para o vetor b
        }
        row++;
    }

    free(line); // Libera memória alocada para a linha
    fclose(file);
    printf("Matriz carregada do arquivo: %s\n", filename);
}


// Função para imprimir a matriz
void print_matrix(double *matrix, double *b, int n) {
    printf("Matriz A|b:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%6.2f ", matrix[i * n + j]); // Acesso como vetor unidimensional
        }
        printf("| %6.2f\n", b[i]); // Imprime o elemento correspondente no vetor b
    }
    printf("\n");
}

// Função para imprimir a solução
void print_solution(double *x, int n) {
    printf("Solução do sistema (x):\n");
    for (int i = 0; i < n; i++) {
        printf("x[%d] = %6.2f\n", i, x[i]);
    }
    printf("\n");
}

// Método de eliminação de Gauss
void gaussian_elimination(double *matrix, double *b, double *x, int n, int rank, int size) {
    for (int k = 0; k < n; k++) {
        int pivot_owner = k % size;

        if (rank == pivot_owner) {
            // Normalizando a linha pivô
            for (int j = k + 1; j < n; j++) {
                matrix[k * n + j] /= matrix[k * n + k];
            }
            b[k] /= matrix[k * n + k];
            matrix[k * n + k] = 1.0;
        }

        // Compartilhando linha pivô e vetor b entre os processos
        MPI_Bcast(&matrix[k * n], n, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);
        MPI_Bcast(&b[k], 1, MPI_DOUBLE, pivot_owner, MPI_COMM_WORLD);

        // Atualização das linhas da matriz pelos processos
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

        // Sincronização opcional (para debug ou análise intermediária)
        
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            printf("Matriz após etapa %d:\n", k);
            print_matrix(matrix, b, n);
        }
        
    }

    // Substituição reversa para encontrar as soluções
    for (int i = n - 1; i >= 0; i--) {
        if (rank == i % size) {
            x[i] = b[i];
            for (int j = i + 1; j < n; j++) {
                x[i] -= matrix[i * n + j] * x[j];
            }
        }
        MPI_Bcast(&x[i], 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
    }

    // Sincronização opcional (para verificar soluções parciais)
    /*
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Solução parcial após substituição reversa:\n");
        print_solution(x, n);
    }
    */
}

int main(int argc, char **argv) {
    int rank, size, n = N;
    double *matrix;
    double *b;
    double *x;
    double t_inicial, t_final;

    if (argc > 1) {
        n = atoi(argv[1]);
    }

    // Aloca memória para a matriz e vetores
    matrix = (double *) malloc(n * n * sizeof(double));
    b = (double *) malloc(n * sizeof(double));
    x = (double *) malloc(n * sizeof(double));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Processo %d de %d iniciado\n", rank, size);

    if (rank == 0) {
        // Carrega a matriz do arquivo
        const char *filename = "matrix.txt";
        load_matrix_from_file(filename, matrix, b, n);
    }

    // Broadcast dos dados
    MPI_Bcast(matrix, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Impressão inicial
    /* if (rank == 0) {
        printf("Matriz inicial carregada (A|b):\n");
        print_matrix(matrix, b, N);
    } */

    t_inicial = MPI_Wtime();

    // Executa eliminação de Gauss
    gaussian_elimination(matrix, b, x, n, rank, size);

    t_final = MPI_Wtime();

    if (rank == 0) {
        printf("\nSolução final (x):\n");
        print_solution(x, n);
        printf("Tempo de execução: %.6f segundos\n", t_final - t_inicial);
    } 

    MPI_Finalize();
    return 0;
}

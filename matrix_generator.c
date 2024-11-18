#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 500 // Tamanho da matriz

// Função para salvar a matriz no arquivo
void save_matrix_to_file(const char *filename, double matrix[N][N], double b[N], int n) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir o arquivo para escrita");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(file, "%.2f", matrix[i][j]);
            if (j < n - 1) fprintf(file, ",");
        }
        fprintf(file, ";%.2f\n", b[i]);
    }

    fclose(file);
    printf("Matriz de tamanho %d salva no arquivo: %s\n", n, filename);
}

// Função para gerar valores aleatórios na matriz e vetor b
void generate_random_matrix(double matrix[N][N], double b[N], int n) {
    srand(time(NULL)); // Semente para números aleatórios

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (rand() % 200 - 100) / 10.0; // Valores entre -10.0 e 10.0
        }
        b[i] = (rand() % 200 - 100) / 10.0; // Valores entre -10.0 e 10.0
    }
}

int main(int argc, char **argv) {

    double matrix[N][N];
    double b[N];

    // Gera matriz aleatória e vetor b
    generate_random_matrix(matrix, b, N);

    // Salva a matriz e o vetor no arquivo
    const char *filename = "matrix.txt";
    save_matrix_to_file(filename, matrix, b, N);

    return 0;
}
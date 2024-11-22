#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 3 // Tamanho da matriz

// Função para salvar a matriz no arquivo
void save_matrix_to_file(const char *filename, double *matrix, double *b, int n) {
    FILE *file = fopen(filename, "w");
    srand(time(NULL));
    if (!file) {
        perror("Erro ao abrir o arquivo para escrita");
        exit(EXIT_FAILURE);
    }
    int j = 0;
    for (int i = 0; i < n*n; i++) {
        fprintf(file, "%.2f", (rand() % 200 - 100) / 10.0);
        j = i % n;
        if (j < n - 1) fprintf(file, ",");
        else fprintf(file, ";%.2f\n",  (rand() % 200 - 100) / 10.0);
    }

    fclose(file);
    printf("Matriz de tamanho %d salva no arquivo: %s\n", n, filename);
}

int main(int argc, char **argv) {

    double *matrix;
    double *b;
    int n = N;

    // Aloca memória para a matriz e vetor b
    matrix = (double *) malloc(N * N * sizeof(double));
    b = (double *) malloc(N * sizeof(double));

    // Salva a matriz e o vetor no arquivo
    const char *filename = "matrix.txt";
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    save_matrix_to_file(filename, matrix, b, n);

    return 0;
}
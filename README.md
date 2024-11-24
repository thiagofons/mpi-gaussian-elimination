## Instruções:

- Instruções para o gerador de Matrizes

    - Para rodar a aplicação, primeiro é necessário compilar o arquivo **matrix_generator.c**:

    ``` gcc ./matrix_generator.c -o mg ```

    - Após compilar, é possível executar o gerador de matrizes com o seguinte comando:

    ``` ./mg [tamanho-da-matriz-n-x-n] ```

    - O programa irá gerar um arquivo chamado "matrix.txt", contendo a matriz N x N no formado que pode ser lido pelos resolvedores de sistemas lineares.

- Instruções para o resolvedor por eliminação Gaussiana feito por IA

    - Para rodar a aplicação de eliminação gaussiana, é necessário compilar o arquivo **main_gpt.c**:

    ``` mpicc ./main_gpt.c -o mpi_gaussian_gpt ```

    - Após compilar, é possível executar a aplicação a partir do seguinte comando:

    ``` mpirun -np [numero-de-processos] ./mpi_gaussian_gpt [tamanho-da-matriz-n-x-n] ```

- Instruções para o resolvedor por eliminação Gaussiana feito pelo grupo

    - Para rodar a aplicação de eliminação gaussiana, é necessário compilar o arquivo **main_human.c**:

    ``` mpicc ./main_human.c -o mpi_gaussian_human ```

    - Após compilar, é possível executar a aplicação a partir do seguinte comando:

    ``` mpirun -np [numero-de-processos] ./mpi_gaussian_human [tamanho-da-matriz-n-x-n] ```

## Exemplo de uso:

```
gcc ./matrix_generator.c -o mg
./mg 4

mpicc ./main_gpt.c -o mpi_gaussian_gpt
mpirun -np 2 ./mpi_gaussian_gpt 4

mpicc ./main_human.c -o mpi_gaussian_human
mpirun -np 2 ./mpi_gaussian_human 4
```

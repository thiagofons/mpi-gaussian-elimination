## Instruções:

- Para rodar a aplicação, primeiro é necessário compilar o arquivo **matrix_generator.c** com o seguinte comando:

 ``` gcc ./matrix_generator.c -o mg ```

- Após compilar, rodar o seguinte comando:

``` ./mg [tamanho-da-matriz-n-x-n] ```

- Agora, para rodar a aplicação de eliminação gaussiana, é necessário compilar o arquivo **main.c** com o seguinte comando:

``` mpicc ./main.c -o mpi_gaussian ```

- E, após compular, rodar o seguinte comando:

``` mpirun -np [numero-de-processos] ./mpi_gaussian [tamanho-da-matriz-n-x-n] ```

## Exemplo de uso:

```
gcc ./matrix_generator.c -o mg
./mg 4
mpicc ./main.c -o mpi_gaussian
mpirun -np 2 ./mpi_gaussian 4
```

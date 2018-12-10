#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define ONE_OVER_THOUSAND 1E-3 
#define MILION 1E6

int rows_calculated = 0;

typedef struct {
  int **matrix1;
  int **matrix2;
  int **result;
  int size;
} Zad2Matrices;

void *squared_boolean_matrix_multithread_multiplication(void *vargp) {
  Zad2Matrices *matrix = (Zad2Matrices*)&vargp;

  for (int i = rows_calculated ; i < rows_calculated; rows_calculated++) {
    for (int j = 0; j < matrix->size; j++) {
      int value = 0;
      for (int k = 0; k < matrix->size; k++) {
        value |= (matrix->matrix1[i][k] & matrix->matrix2[k][j]);
        if (value) {
          break;
        }
      }
      matrix->result[i][j] = value;
    }
  }
}

int ** malloc_matrix(int width, int height) {
  int **matrix = (int **)malloc(height * sizeof(int *)); 
  for (int i = 0; i < height; i++) {
    matrix[i] = (int *)malloc(width * sizeof(int));
  }
  return matrix;
}

void random_fill_boolean_matrix(int **matrix, int width, int height) {
  time_t t;
  srand(time(&t));
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      matrix[i][j] = (rand()%2);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("argument error\n");
    exit(1);
  }

  int matrix_size = atoi(argv[1]);
  int num_of_threads = atoi(argv[2]);

  Zad2Matrices matrix; 

  matrix.matrix1 = malloc_matrix(matrix_size, matrix_size);
  matrix.matrix2 = malloc_matrix(matrix_size, matrix_size);
  matrix.result = malloc_matrix(matrix_size, matrix_size);
  matrix.size = matrix_size;

  random_fill_boolean_matrix(matrix.matrix1, matrix_size, matrix_size);
  random_fill_boolean_matrix(matrix.matrix2, matrix_size, matrix_size);

  pthread_t ptIDs[num_of_threads];

  for (int i = 0; i < num_of_threads; i++) {
    pthread_create(&ptIDs[i], NULL, squared_boolean_matrix_multithread_multiplication, (void *)&matrix);
  }

  return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#define ONE_OVER_THOUSAND 1E-3 
#define MILION 1E6

void swap(int *a, int *b) {
  *a = *a + *b;
  *b = *a - *b;
  *a = *a - *b;
}

void print_matrix(int **matrix, int width, int height) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      printf("{%d}", matrix[i][j]);
    }
    printf("\n");
  }
}

void random_fill_matrix(int **matrix, int width, int height) {
  time_t t;
  srand(time(&t));
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      matrix[i][j] = rand()%100;
    }
  }
}

void zero_fill_matrix(int **matrix, int width, int height) {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      matrix[i][j] = 0;
    }
  }
}

void squared_matrix_multiplication(int **matrix1, int **matrix2, int size, int **result) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        result[i][j] += matrix1[i][k]*matrix2[k][j];
      }
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

void transposition(int **matrix, int size) {
  for (int i = 0; i < size - 1; i++) {
    int j = 0;
    for (int k = i + 1; k < size; k++) {
      swap(&matrix[i][k], &matrix[k][i]);
    }
  }
}

int main(int argc, char *argv[]) {
  int matrix_size = atoi(argv[1]);
  printf("matrix_size: %d\n", matrix_size);

  // allocing memory for matrices
  int **matrix1 = malloc_matrix(matrix_size, matrix_size);
  int **matrix2 = malloc_matrix(matrix_size, matrix_size);

  // filling matrices with random numbers
  random_fill_matrix(matrix1, matrix_size, matrix_size);
  random_fill_matrix(matrix2, matrix_size, matrix_size);
  
  // print_matrix(matrix2, matrix_size, matrix_size);

  int **product_matrix = malloc_matrix(matrix_size, matrix_size);
  zero_fill_matrix(product_matrix, matrix_size, matrix_size);

  struct timespec start, end;
  
  clock_gettime(CLOCK_REALTIME, &start);
  time_t t1ns = start.tv_nsec/MILION;
  time_t t1s = start.tv_sec;
  
  transposition(matrix2, matrix_size);
  squared_matrix_multiplication(matrix1, matrix2, matrix_size, product_matrix);
  
  clock_gettime(CLOCK_REALTIME, &end);
  time_t t2ns = end.tv_nsec/MILION;
  time_t t2s = end.tv_sec;

  printf("t1s: %d, t1ns: %d, t2s: %d, t2ns: %d.\n", t1s, t1ns, t2s, t2ns);
  double accum = (t2s - t1s) + (t2ns - t1ns) * ONE_OVER_THOUSAND;
  printf("time: %.*lfs\n", 3, accum);
  // print_matrix(product_matrix, matrix_size, matrix_size);
  return 0;
}
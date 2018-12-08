#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define ONE_OVER_THOUSAND 1E-3 
#define MILION 1E6

void swap(int *a, int *b) {
  *a = *a + *b;
  *b = *a - *b;
  *a = *a - *b;
}

void swap_pointers(int **a, int **b) {
  int *temp = *a;
  *a = *b;
  *b = temp;
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

int ** malloc_matrix(int width, int height) {
  int **matrix = (int **)malloc(height * sizeof(int *)); 
  for (int i = 0; i < height; i++) {
    matrix[i] = (int *)malloc(width * sizeof(int));
  }
  return matrix;
}

void transposition(int **matrix, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int k = i + 1; k < size; k++) {
      swap(&matrix[i][k], &matrix[k][i]);
    }
  }
}

void transpose_matrix(int **matrix, int size, int **result) {
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      // result[i][j] = matrix[j][i];
      int *p1 = &result[i][j];
      int *p2 = &matrix[j][i];
      swap_pointers(&p1, &p2);
    }
  }
}

void diagonal_matrix_multiplication(int **matrix1, int **matrix2, int size, int **result) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        result[i][j] += matrix1[i][k]*matrix2[k][j];
      }
    }
  }
}

void diagonal_matrix_multiplication_with_transposition(int **matrix1, int **matrix2, int size, int **result) {
  int **transposed = malloc_matrix(size, size);
  transpose_matrix(matrix2, size, transposed);

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        result[i][j] += transposed[i][k]*matrix2[j][k];
      }
    }
  }
}

void multiply_and_show_time(int **matrix1, int **matrix2, int matrix_size, int **product_matrix) {
  struct timespec start, end;
  
  clock_gettime(CLOCK_REALTIME, &start);
  time_t t1ns = start.tv_nsec/MILION;
  time_t t1s = start.tv_sec;
  
  diagonal_matrix_multiplication(matrix1, matrix2, matrix_size, product_matrix);
  
  clock_gettime(CLOCK_REALTIME, &end);
  time_t t2ns = end.tv_nsec/MILION;
  time_t t2s = end.tv_sec;

  double accum = (t2s - t1s) + (t2ns - t1ns) * ONE_OVER_THOUSAND;
  printf("standard multiplication\n");
  printf("time: %.*lfs\n", 3, accum);
}

void trans_multiply_and_show_time(int **matrix1, int **matrix2, int matrix_size, int **product_matrix) {
  struct timespec start, end;
  
  clock_gettime(CLOCK_REALTIME, &start);
  time_t t1ns = start.tv_nsec/MILION;
  time_t t1s = start.tv_sec;
  
  diagonal_matrix_multiplication_with_transposition(matrix1, matrix2, matrix_size, product_matrix);
  
  clock_gettime(CLOCK_REALTIME, &end);
  time_t t2ns = end.tv_nsec/MILION;
  time_t t2s = end.tv_sec;

  double accum = (t2s - t1s) + (t2ns - t1ns) * ONE_OVER_THOUSAND;
  printf("multiplication with transposition\n");
  printf("time: %.*lfs\n", 3, accum);
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

  // allocating memory for product matrix and filling with zeros  
  int **product_matrix = malloc_matrix(matrix_size, matrix_size);
  zero_fill_matrix(product_matrix, matrix_size, matrix_size);

  multiply_and_show_time(matrix1, matrix2, matrix_size, product_matrix);

  trans_multiply_and_show_time(matrix1, matrix2, matrix_size, product_matrix);

  return 0;
}
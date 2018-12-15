#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*
The following structure contains the necessary information to allow
the function "squared_boolean_matrix_multithread_multiplication"
to access its input data and place its output into the structure.
*/
typedef struct {
  int **matrix1;
  int **matrix2;
  int **result;
  int size;
} DATA;

/* Declare globally accessible variables and the mutex. */
DATA matrix;
int available_row = 0;
pthread_mutex_t mutex_counter;

/*
This function is activated when the thread is created.
All input is obtained from a structure of type DATA.
The output - product matrix - is also written into this structure.
The function calculate every first available row.
*/
void *squared_boolean_matrix_multithread_multiplication(void *vargp) {
  int i;
  while(available_row < matrix.size) {
    pthread_mutex_lock (&mutex_counter);
      i = available_row;
      available_row++;
    pthread_mutex_unlock (&mutex_counter);
    for (int j = 0; j < matrix.size; j++) {
      int value = 0;
      for (int k = 0; k < matrix.size; k++) {
        value |= (matrix.matrix1[i][k] & matrix.matrix2[k][j]);
        if (value) {
          break;
        }
      }
      matrix.result[i][j] = value;    
    }
  }
  pthread_exit((void*) 0);
}

/* 
This function allocate memory for the int matrix 
for given width and height.
*/
int ** malloc_matrix(int width, int height) {
  int **matrix = (int **)malloc(height * sizeof(int *)); 
  for (int i = 0; i < height; i++) {
    matrix[i] = (int *)malloc(width * sizeof(int));
  }
  return matrix;
}

/*
This function randomly fill given matrix with 0 or 1.
*/
void random_fill_boolean_matrix(int **matrix, int width, int height) {
  time_t t;
  srand(time(&t));
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      matrix[i][j] = (rand()%2);
    }
  }
}

void free_2d_array(int **matrix, int height) {
  for (int i = 0; i < height; i++) {
    free(matrix[i]);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("argument error\n");
    exit(1);
  }

  int matrix_size = atoi(argv[1]);
  int num_of_threads = atoi(argv[2]);
  printf("size: %d, num of threads: %d\n", matrix_size, num_of_threads);
  printf("calculating...\n");

  // initialising the DATA struct
  matrix.matrix1 = malloc_matrix(matrix_size, matrix_size);
  matrix.matrix2 = malloc_matrix(matrix_size, matrix_size);
  matrix.result = malloc_matrix(matrix_size, matrix_size);
  matrix.size = matrix_size;

  random_fill_boolean_matrix(matrix.matrix1, matrix_size, matrix_size);
  random_fill_boolean_matrix(matrix.matrix2, matrix_size, matrix_size);

  pthread_attr_t attr;
  pthread_t ptIDs[num_of_threads];
  pthread_mutex_init(&mutex_counter, NULL);        
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  void *status;

  for (int i = 0; i < num_of_threads; i++) {
    pthread_create(&ptIDs[i], &attr, squared_boolean_matrix_multithread_multiplication, NULL);
  }

  // waiting on the other threads
  for (int i = 0; i <num_of_threads; i++) {
    pthread_join(ptIDs[i], &status);
  }

  // cleanup
  pthread_mutex_destroy(&mutex_counter);
  free_2d_array(matrix.matrix1, matrix_size);
  free_2d_array(matrix.matrix2, matrix_size);
  free_2d_array(matrix.result, matrix_size);
  return 0;
}
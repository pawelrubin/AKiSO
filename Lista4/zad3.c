#define DELIM " \t\r\a"
#define _GNU_SOURCE
#define PROMPT_COLOR "\033[36;1m"
#define COLOR "\033[33;0m"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

int count = 1;
char * directory;
int amp;

void dibadzer(char *args[], int maksLen) {
  printf("counter: %d\n", count);
  printf("maksLen: %d\n", maksLen);
  for (int i = 0; i < count; i ++) {
    printf("%s ", args[i]);
  } 
  printf("\n");
  printf("ELO\n");
}

void cd(char *dir) {
  if (chdir(dir) < 0) {
    perror("cd failed.");
  } else {
    directory = dir;
  }
}

void sigintHandler(int sig_num) { 
    signal(SIGINT, sigintHandler); 
    char *arg[] = {"clear"};
    execvp("clear",arg);
    printf("\n");
}

int ampersand(char *args[]) {
  int i = 0;
  while(args[i] != NULL) {
    if (strcmp(args[i],"&") == 0) {
      if (args[i + 1] == NULL) {
        args[i] = NULL;
        return 1;
      }
    }
    i++;
  }
  return 0;
}

void execution(char *args[]) {
  int status;
  amp = ampersand(args);
  if (*args != NULL) {
    if (strcmp(args[0], "cd") == 0) {
      cd(args[1]);
    } else {
      pid_t child;
      child = fork();
      if (child == 0) {
        if (execvp(args[0], args) == -1) {
          perror("I can't recognize your command");
        }
        _exit(EXIT_SUCCESS);
      } else {
        if(amp == 0) {
          int wpid = waitpid(child, &status, 0);
        } else {
          signal(SIGCHLD, SIG_IGN);
        }
      } 
    }
  }
}

char** split(char *string, char *delim) {
  char** result;
  int i = 0, j = 0;
  int len = 0;
  int maksLen = 0;
  int precount;
  count = 1;
  // counting number of tokens
  while(string[i] != '\0') {
    j = 0;
    precount = count;
    while(delim[j] != '\0') {
      if (string[i] == delim[j]) {
        count++;
        break;
      }
      j++;
    }

    if (precount == count) {
      len++;
      if (len > maksLen) {
        maksLen = len;
      }
    } else {
      len = 0;
    }
      
    i++;
  }
  
  result = malloc(maksLen * count * sizeof(char*));
  // count++; // ? :(

  // spliting into tokens
  if (result != NULL) {
    size_t idx = 0;
    char* token = strtok(string, delim);

    while (token != NULL) {
        result[idx] = token;
        token = strtok(0, delim);
        idx++;
    }
    result[idx] = NULL;
  }

  dibadzer(result, maksLen);
  return result;
}

void commandLoop() {
  int status = 1;
  char *line = NULL;
  char **args = NULL;
  size_t len = 0;
  ssize_t nread;
  char directory[256];
  do { 
    printf("%slshmordo@%s> ", PROMPT_COLOR, getcwd(directory, sizeof(directory)));
    printf("%s", COLOR);
    nread = getline(&line, &len, stdin);
    
    if (strcmp(line, "exit") == 0 || nread == -1) {
      exit(0);
    }

    if (strlen(line) == 1) {
      continue;
    }

    if (strlen(line) > 0 && line[strlen(line) - 1] == '\n') {
      line[strlen(line)-1] = '\0';
    }

    args = split(line, DELIM);
    execution(args);    

    int i = 0;
    while (0 < --i) free(args[i]);
  } while (1);
}

int main() {
  signal(SIGINT, sigintHandler); 
  commandLoop();
  return 0;
}
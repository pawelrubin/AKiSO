#define DELIM " \t\r\a"
#define PIPE_DELIM "|"
#define _GNU_SOURCE
#define PROMPT_COLOR "\033[36;1m"
#define COLOR "\033[33;0m"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/limits.h>
int count = 1;
char * directory;
int amp;
int sizeAwesome;
int commandNumAwesome;

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

char** split(char *string, char *delim, int *num, size_t *sizeOfArray) {
  if (string == NULL) {
    return NULL;
  }
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
  int size = maksLen * count * sizeof(char*);
  *sizeOfArray = maksLen * count * sizeof(char*);;
  result = malloc(size);
  *num = count;

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

  // dibadzer(result, maksLen);
  return result;
}

char*** splitAwesome(char *string, int *num) {
  int size;
  size_t arraySize;
  char **pipedCommands = split(string, PIPE_DELIM, &size, &arraySize);
  amp = ampersand(pipedCommands);
  *num = 0;
  // for (int i = 0; i < size; i++) {
  //   printf("%sPIPEDELO", pipedCommands[i]);
  // }
  char *** result = malloc(arraySize * size);
  // sizes = malloc(size * 256);
  printf("\n%d\n", size);
  for (int i = 0; i < size; i++) {
    int s;
    size_t aS;
    result[i] = split(pipedCommands[i], DELIM, &s, &aS);
    amp = ampersand(result[i]);
    *num += s;
    // *sizes[i] = s;
      for (int j = 0; j < s; j++) {
        printf("%sELO", result[i][j]);
      }
      printf("\n");
  }
  
  return result;
}

void execAwesome(char *line) {
  int size;
  char ***commands = splitAwesome(line, &size);
  printf("SIZE: %d\n\n",size);
  printf("AMP: %d\n\n", amp);
  pid_t child = fork();
  int status, wpid;
  if (child == 0) {
    int fd[size-1][2];
    
    for (int i = 0; i < size - 1; i++) {
      if (pipe(fd[i]) == -1) {
        perror("pipe error");
      }
    }

    int pids[size];
    

    for (int i = 0; i < size; i++) {
      if ((pids[i] = fork()) == 0) {
        printf ("pids[%d] = fork() = 0 ELO\n", i);
        if (size > 1) {
          if (i == 0) {
            // case for the LAST command in pipes
            if (dup2(fd[i][0], 0) == -1) {
              perror("dup2 error on i=0");
            }
            
            addRedirects(size, commands, i, 1);
            
          } else if (i != size - 1) {
            // case for all the MIDDLE commands in pipes
            if (dup2(fd[i-1][1], 1) == -1) {
              perror("dup2 error");
            }
            if (dup2(fd[i][0], 0) == -1) {
              perror("dup2 error");
            }
            
            addRedirects(size, commands, i, -1);
            
          } else {
            // case for the FRIST command in pipes
            if (dup2(fd[i-1][1], 1) == -1) {
              perror("dup2 error on i=commands-1");
            }
            
            addRedirects(size, commands, i, 0);
          }
        } else {
          // if we have only one command, we add all redirects to it
          addRedirects(size, commands, 0, 2);
        }

        // we close all unused fds - important!
        for (int j = 0; j < size - 1; j++) {
          close(fd[j][0]);
          close(fd[j][1]);
        }
        
        // we execute the command - process is replaced by it and dies off eventually, freeing resources
        execvp(commands[i][0], commands[i]);
        // if we get to this part, execvp failed
        perror("execvp error");
        _exit(1);
      }
    }
    printf("po petli ELO \n");
    // close fds in parten
    for (int j = 0; j < size - 1; j++) {
      close(fd[j][0]);
      close(fd[j][1]);
    }
    
    // if we decided to wait, wait for all
    if(amp == 0) {
      for(int i = size - 1; i > -1; i--) {
        if(pids[i] > 0) {
          int status;
          waitpid(pids[i], &status, 0);
        } else {
          // if pid isn't greater than 0 then... what
        }
      }
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

void commandLoop() {
  int status = 1;
  int num;
  char *line = NULL;
  char **args = NULL;
  char ***awesome = NULL;
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

    char tmpLine[len];
    strcpy(tmpLine, line);
    // awesome = splitAwesome(tmpLine);
    execAwesome(tmpLine);
    // strcpy(tmpLine, line);
    // args = split(tmpLine, DELIM, &num);
    
    // execution(args);    

    int i = 0;
    // while (0 < --i) free(args[i]);
  } while (1);
}

int main() {
  signal(SIGINT, sigintHandler); 
  commandLoop();
  return 0;
}

void addRedirects(int size, char* commands[][size], int index, int mode) {
	
	int commandLength = 0;
	while (commands[index][commandLength] != NULL) commandLength++;
	
	for(int j = 0; j < commandLength; j++) {
		
		if(commands[index][j][0] == '>' && (mode == 1 || mode == 2)) {
			
			char path[PATH_MAX];
			for(int k = 1; k < strlen(commands[index][j]); k++) {
				path[k-1] = commands[index][j][k];
			}
			
			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("> error");
				_exit(1);
			}
			
			if (dup2(file, 1) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}
						
			commands[index][j] = NULL;
							
		} else if (commands[index][j][0] == '2' && commands[index][j][1] == '>') {
					
			char path[PATH_MAX];
			for(int k = 2; k < strlen(commands[index][j]); k++) {
				path[k-2] = commands[index][j][k];
			}
						
			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("2> error");
				_exit(1);
			}
			if (dup2(file, 2) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}
					
			commands[index][j] = NULL;
			
		} else if (commands[index][j][0] == '<' && (mode == 0 || mode == 2)) {
			
			char path[PATH_MAX];
			for(int k = 1; k < strlen(commands[index][j]); k++) {
				path[k-1] = commands[index][j][k];
			}
			
			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("> error");
				_exit(1);
			}
			
			if (dup2(file, 0) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}
						
			commands[index][j] = NULL;

		}
	}
}
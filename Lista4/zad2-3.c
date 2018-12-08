#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int counter = 0;

void sigusr1_handler(int signal_number) {
  printf("Odebrałem %d\n", counter);
  counter++;
}

int main() {
  pid_t pid;
  pid = fork();
  
  if (pid == 0) {
    for (int i = 0; i < 100; i++) {
      printf("Wysyłam %d\n", i);
      kill(getppid(), SIGUSR1);
    }
    _exit(0);// dreturn 0;
  } else {
    signal(SIGUSR1, sigusr1_handler);
    // while(1);
  } 
  return 0;
}
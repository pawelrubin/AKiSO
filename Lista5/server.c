
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/time.h>
#include <arpa/inet.h>

#define PORT 8888 
#define MAX_CLIENTS 30
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 128

typedef struct {
  int sockets[MAX_CLIENTS];
  char nicknames[MAX_CLIENTS][30];
} Clients;

typedef struct {
  Clients clients;
  // int sockets[MAX_CLIENTS];
  int master_socket;
  int opt;
  struct sockaddr_in address;
  int addr_len;
  fd_set readfds;
  fd_set writefds;
  int max_sd;
  int sd;
  int activity;
  int new_socket;
} Server;

// Globally accesible structure of data
Server server;

void initialise_all_to_0(int *array, int size) {
  for (int i = 0; i < size; i++) {
    array[i] = 0;
  }
}

char * int_to_string(int number, int base) {
  int num_of_digits = 0;
  int number_copy;
  int isNegative = 0;  
  
  if (number == 0) {
    return "0";
  }

  if (number < 0) {
    isNegative = 1;
    number *= (-1);
  }

  number_copy = number;

  while(number_copy > 0) {
    number_copy /= base;
    num_of_digits++;
  }

  number_copy = number;

  char *result = malloc((num_of_digits + 1 + isNegative) * sizeof(char));

  if (isNegative) {
    result[0] = '-';
  }

  if (base <= 10) {
    for(int i = (num_of_digits - 1 + isNegative); i >= isNegative; i--) {
      result[i] = number_copy%base + '0';
      number_copy /= base;
    }
  } else {
    char digits[] = {'A', 'B', 'C', 'D', 'E', 'F'};
    int digit;
    for(int i = (num_of_digits - 1 + isNegative); i >= isNegative; i--) {
      digit = number_copy%base;
      if (digit < 10) {
        result[i] = digit + '0';
      } else {
        result[i] = digits[digit%10];
      }
      number_copy /= base;
    }
  }

  result[num_of_digits + isNegative] = '\0';

  return result;
}

void show_users(int socket) {
  int count = 1;
  char *message = "\033[1m\nConnected users:\n";
  send(socket, message, strlen(message), 0);
  for (int j = 0; j < MAX_CLIENTS; j++) {
    if (server.clients.sockets[j] != 0) {
      if (server.clients.sockets[j] != socket) {
        char nickname[30];
        strncpy(nickname, "", sizeof(nickname));
        strcat(nickname, server.clients.nicknames[j]);
        message = int_to_string(count, 10);
        // strncpy(message, int_to_string(count, 10), sizeof(message));
        strcat(message, ". ");
        strcat(message, nickname);
        strcat(message, "\n");
        count++;
      } else {
        message = int_to_string(count, 10);
        strcat(message, ". ");
        strcat(message, "You\n");
        count++;
      }
      send(socket, message, strlen(message), 0);
    }
  }
  send(socket, "\n\033[0m", strlen("\n\033[0m"), 0);
}

void user_connected_notification(int id) {
  char *new_user_nickname = server.clients.nicknames[id];

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (i != id) {
      int socket = server.clients.sockets[i];
      char message[128];
      strncpy(message, "\n\033[1m", sizeof(message));
      strcat(message, new_user_nickname);
      strcat(message, " connected\033[0m\n\n");
      send(socket, message, strlen(message), 0);
    }
  }
}

/*
This function returns a socket for a given nickname
if the nickname has been found, otherwise returns -1.
*/
int find_socket_by_nickname(char *nickname) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (strcmp(server.clients.nicknames[i], nickname) == 0) {
      return server.clients.sockets[i];
    }
  }
  free(nickname);
  return -1;

}

char *first_word(char *buffer) {
  int inputLength = strlen(buffer);
  char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
  strncpy(inputCopy, buffer, inputLength);
  char *word, *context;
  word = strtok_r(inputCopy, " ", &context);
  return word;
}

void send_to_user(int socket, char *from_user, char *message) {
  send(socket, from_user, strlen(from_user), 0);
  send(socket, ": ", strlen(": "), 0);
  send(socket, message, strlen(message), 0);
  send(socket, "\n", strlen("\n"), 0);
}

int main(int argc, char const *argv[]) { 
  
  char buffer[BUFFER_SIZE];
  char nick[30];
  int started[MAX_CLIENTS];
  server.opt = TRUE;
  server.master_socket = socket(AF_INET, SOCK_STREAM, 0);
  server.address.sin_family = AF_INET;   
  server.address.sin_addr.s_addr = INADDR_ANY;   
  server.address.sin_port = htons(PORT);   
  server.addr_len = sizeof(server.address);

  initialise_all_to_0(server.clients.sockets, MAX_CLIENTS);

  setsockopt(server.master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&server.opt, sizeof(server.opt));     
         
  bind(server.master_socket, (struct sockaddr *)&server.address, sizeof(server.address));
  printf("Listener on port %d \n", PORT);

  listen(server.master_socket, 3);

  puts("Waiting for connections");

  while (TRUE) {
    FD_ZERO(&server.readfds);

    FD_SET(server.master_socket, &server.readfds);
    FD_SET(server.master_socket, &server.writefds);
    server.max_sd = server.master_socket;

    for (int i = 0; i < MAX_CLIENTS; i++) {

      server.sd = server.clients.sockets[i];
      //if valid socket descriptor then add to read list  
      if(server.sd > 0)   
        FD_SET(server.sd, &server.readfds);   
            
      //highest file descriptor number, need it for the select function  
      if (server.sd > server.max_sd)   
        server.max_sd = server.sd;   
    }

    select(server.max_sd + 1, &server.readfds, &server.writefds, NULL, NULL);

    if (FD_ISSET(server.master_socket, &server.readfds)) {
      server.new_socket = accept(server.master_socket, (struct sockaddr *)&server.address,(socklen_t*)&server.addr_len);
      printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
        server.new_socket, inet_ntoa(server.address.sin_addr), ntohs(server.address.sin_port));
      
      // welcome message
      char *message = "\n*************\n ELO MORDZIU\n*************\nPodaj nick: ";
      send(server.new_socket, message, strlen(message), 0);
      puts("WELCOME MESSAGE HAS BEEN SEND. POZDRO.");
      
      // reading nickname
      int len = read(server.new_socket, nick, sizeof(nick));
      nick[len-2] = '\0';

      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server.clients.sockets[i] == 0) {
          server.clients.sockets[i] = server.new_socket;
          strncpy(server.clients.nicknames[i], nick, sizeof(server.clients.nicknames[i]));
          printf("Adding to list of sockets as %d\n" , i);
          printf("nickname is : <%s>\n", server.clients.nicknames[i]);
          
          char greet[50];
          strcpy(greet, "Welcome ");
          strcat(greet, nick);
          strcat(greet, "\n");
          send(server.clients.sockets[i], greet, strlen(greet), 0);

          user_connected_notification(i);
          show_users(server.clients.sockets[i]); 

          break;
        }
      }

      
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {   
      server.sd = server.clients.sockets[i];   
            
      if (FD_ISSET(server.sd, &server.readfds)) {
        int len;
        if ((len = recv(server.sd, buffer, sizeof(buffer), 0)) == 0) {   
            getpeername(server.sd, (struct sockaddr*)&server.address,(socklen_t*)&server.addr_len);   
            printf("Host disconnected , ip %s , port %d \n", inet_ntoa(server.address.sin_addr), ntohs(server.address.sin_port));   
            close(server.sd);   
            server.clients.sockets[i] = 0;
            for (int j = 0; j < MAX_CLIENTS; j++) {
            if (j != i) {
              int a_socket = server.clients.sockets[j];
              char *client = server.clients.nicknames[i];
              char *dis = " disconnected\n";
              send(a_socket, client, strlen(client), 0);
              send(a_socket, dis, strlen(dis), 0);
            }
          }
        } else {
          buffer[len - 2] = '\0';
          int target_socket;
          char *nickname = first_word(buffer);
          if ((target_socket = find_socket_by_nickname(nickname)) > -1) {
            printf("wiadomosc od <%s> do <%s>: <%s>\n", server.clients.nicknames[i], nickname, buffer + strlen(nickname) + 1);
            send_to_user(target_socket, server.clients.nicknames[i], buffer + strlen(nickname) + 1);
          } else if (strcmp(nickname, "users")) {
            show_users(server.clients.sockets[i]);
          } else {
            for (int j = 0; j < MAX_CLIENTS; j++) {
              if (j != i) {
                int a_socket = server.clients.sockets[j];
                if (a_socket != 0) {
                  send_to_user(a_socket, server.clients.nicknames[i], buffer);
                  printf("wiadomosc od <%s> do wszystkich: <%s>\n", server.clients.nicknames[i], buffer);
                }
              }
            }
          }
        }
      } 
    }
  }

  return 0;
}


#include "shared.c" // Assegure-se que este arquivo contém as definições de item_t e shared_t
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

/*
Lucas de Sousa Marcato - 21003317
Antonio Marcio Crepaldi Junior - 21012485
Bianca Aparecida Andrade - 21007245
João Gabriel de Campos Rosa - 21003827
Eduardo Augusto Marras de Sousa - 20078408
Rafael Vedoato lett - 21000375
*/

void consumidor(item_t *prox, int socket_fd) {
  // Receive message size
  if (recv(socket_fd, &(prox->length), sizeof(prox->length), 0) <= 0) {
    perror("Failed to receive length or connection closed");
    exit(1);
  }

  prox->message = (char *)malloc(prox->length);
  if (prox->message == NULL) {
    perror("\nMemory allocation failed\n");
    exit(1);
  }

  // Receive message
  if (recv(socket_fd, prox->message, prox->length, 0) <= 0) {
    perror("Failed to receive message or connection closed");
    free(prox->message);
    exit(1);
  }
  printf("%s\n", prox->message);
}

int main() {
  int sockfd;
  struct sockaddr_in server_socket_address;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(1);
  }

  memset(&server_socket_address, 0, sizeof(server_socket_address));
  server_socket_address.sin_family = AF_INET;
  server_socket_address.sin_port = htons(PORT);
  server_socket_address.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(sockfd, (struct sockaddr *)&server_socket_address, sizeof(server_socket_address)) < 0) {
    perror("Connection Failed");
    exit(1);
  }

  item_t item;
  while (1) {
    printf("\nWaiting for server message\n");
    consumidor(&item, sockfd);
    if (item.length == 0) {
      printf("\nConnection closed by server or error occurred.\n");
      break;
    }
  }

  free(item.message);
  close(sockfd);
  return 0;
}

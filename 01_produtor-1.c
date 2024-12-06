#include "shared.c"
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

A-) Quando apenas o produtor está em execução, o processo fica bloqueado aguardando a conexão
de um consumidor para poder iniciar a execução. Já no caso contrário, o consumidor não
conseguiria se conectar a um socket e geraria erro.
O comportamento é o esperado, uma vez que o consumidor não conseguiria consumir itens 
sem ter itens consumidos, e no caso do produtor, o buffer poderia ser estourado caso
não tivesse nenhum consumidor para consumir os itens.

B-) Não, pois não é realizado uma cópia da mensagem ou do item em sí, caso outro consumidor se conecte
também, é possível de ter erro de execução por erro de segmentação graças a falta de sincronização.
Algumas sugestões poderiam ser em enviar a CÓPIA da mensagem ao invés do ponteiro dela, ou 
criar sincronização por semáforos, mutex ou com estruturas lock-free (que não façam bloqueio de recursos).
Uma vez utilizada alguma dessas sugestões, até mesmo outras máquinas poderiam tentar consumir do produtor,
por conta do protocolo ser TCP.
*/

// Functions prototype definition.
void produtor(item_t *prox, int socket_fd);

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_socket_address, client_socket_address;
  socklen_t cliaddrlen = sizeof(client_socket_address);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Socket creation failed");
    exit(1);
  }

  memset(&server_socket_address, 0, sizeof(server_socket_address));
  server_socket_address.sin_family = AF_INET;
  server_socket_address.sin_port = htons(PORT);
  server_socket_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr *)&server_socket_address, sizeof(server_socket_address)) < 0) {
    perror("\nBind failed\n");
    exit(1);
  }

  if (listen(server_fd, 1) < 0) {
    perror("\nListen failed\n");
    exit(1);
  }

  printf("\nWaiting Consumer connection.\n");
  client_fd = accept(server_fd, (struct sockaddr *)&client_socket_address, &cliaddrlen);
  if (client_fd < 0) {
    perror("\nConnection refused\n");
    exit(1);
  }
  printf("\nConnection accepted.\n");

  char input[256];
  while (1) {
    printf("\nEnter an input message: ");
    if (!fgets(input, sizeof(input), stdin)) {
      break;
    }
    if (strncmp(input, "sair", 4) == 0) {
      break;
    }

    item_t item;
    item.length = strlen(input) + 1; // Insert '\0' at end
    item.message = input;

    produtor(&item, client_fd);
  }

  close(client_fd); // End client socket server
  close(server_fd); // End socket server
  return 0;
}

void produtor(item_t *prox, int socket_fd) {
  send(socket_fd, &(prox->length), sizeof(prox->length), 0);
  send(socket_fd, prox->message, prox->length, 0);
}
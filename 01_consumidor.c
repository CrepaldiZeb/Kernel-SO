#include "shared.h"
#include <fcntl.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

/*

Lucas de Sousa Marcato - 21003317
Antonio Marcio Crepaldi Junior - 21012485
Bianca Aparecida Andrade - 21007245
João Gabriel de Campos Rosa - 21003827
Eduardo Augusto Marras de Souza -20078408

A -)Quando o consumidor consome itens sem o produtor estar em execução, será
gerado um erro, uma vez que o consumidor não terá itens para consumir e nem
endereço de memória compartilhada para acessar, que é alocado no produtor.

B -)Consumidor pode consumir itens enquanto o produtor continuar produzindo.
Entretanto, o consumidor não pode consumir mais itens do que são produzidos, ou
seja, quando o buffer está vazio.

C -)Assim como o Produtor, não é possível ter múltiplas instâncias. O código atual
NÃO possui mecanismos de sincronização, como semáforos e mutex, e nem é adaptado
para ser lock-free.
*/

// Function Prototype
void consumidor(item_t *prox, shared_t *shared);

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "Portuguese");
  const char *name = "MEM";
  void *ptr;
  int shm_fd = shm_open(name, O_RDONLY, 0666);

  ptr = mmap(0, sizeof(shared_t), PROT_READ, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    close(shm_fd);
    return 1;
  }

  // Converte o endereço vazio em um endereço de shared
  shared_t *my_shared = (shared_t *)ptr;

  while (true) {
    item_t next_item;
    next_item.n = 0;

    printf("\nEnter a integer number to consume\n");
    scanf("%d", &next_item.n);

    if (next_item.n == -1) {
      break;
      return 0;
    }
    consumidor(&next_item, my_shared);
  }

  munmap(my_shared, sizeof(shared_t));
  close(shm_fd);

  return 0;
}

void consumidor(item_t *prox, shared_t *shared) {
  if (shared->in == shared->out) {
    printf("Empty Buffer. Item not consumed");
    sleep(1);
    return;
  }
  *prox = shared->buffer[shared->out];
  shared->out = (shared->out + 1) % BUFFER_SIZE;
}
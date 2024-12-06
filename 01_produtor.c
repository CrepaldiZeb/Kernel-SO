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
Eduardo Augusto Marras de Souza - 20078408
*/
/*
A -)Quando o produtor está produzindo itens sem o consumidor está consumindo,
não existe praticamente problemas de concorrência, até que o buffer fique cheio.
Uma vez cheio, o produtor fica em espera e não pode produzir novos itens no
buffer até que os itens produzidos sejam consumidos.

B -)Seria possível produzir até BUFFER_SIZE-1 itens caso o Consumidor não
estivesse consumindo. Caso exceda o BUFFER_SIZE-1, o produtor fica em espera até
que algum item seja consumido.

C -)Não, pois não existe nenhum mecanismo de sincronização como semáforo ou mutex. Isso poderia
gerar problemas de concorrência com múltiplas instâncias, além 
*/

// Function Prototype
void produtor(item_t *prox, shared_t *shared);

bool is_buffer_full(shared_t *shared) {
  return ((shared->in + 1) % BUFFER_SIZE) == shared->out;
}

int main(int argc, char *argv[]) {
  // Allow UTF-8 char
  setlocale(LC_ALL, "Portuguese");

  const char *name = "MEM";
  // Pointer to shared memory
  void *ptr;

  // Create shared memory segment
  int shm_fd = shm_open(name, O_RDWR | O_CREAT, 0666);

  // Detect if the shared memory segment was created with sucess.
  if (shm_fd < 0) {
    perror("Shared memory error");
    close(shm_fd);
    return 1;
  }
  // Size
  ftruncate(shm_fd, sizeof(shared_t));

  ptr = mmap(0, sizeof(shared_t), PROT_WRITE, MAP_SHARED, shm_fd, 0);

  shared_t *shared_memory = (shared_t *)ptr;
  shared_memory->in = 0;
  shared_memory->out = 0;

  while (true) {
    item_t next_item;
    next_item.n = 0;

    printf("\nEnter a integer number\n");
    scanf("%d", &next_item.n);

    if (next_item.n == -1) {
      break;
      return 0;
    }
    produtor(&next_item, shared_memory);
  }

  munmap(shared_memory, sizeof(shared_t));
  close(shm_fd);
  shm_unlink(name);
  return 0;
}

void produtor(item_t *prox, shared_t *shared) {
  if (is_buffer_full(shared)) {
    printf("Full Buffer");
    return;
  }
  shared->buffer[shared->in] = *prox;
  shared->in = (shared->in + 1) % BUFFER_SIZE;
}
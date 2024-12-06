/*
Lucas de Sousa Marcato -21003317
Antonio Marcio Crepaldi Junior-21012485
Bianca Aparecida Andrade -21007245
João Gabriel de Campos Rosa -21003827
Eduardo Augusto Marras de Souza -20078408
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>

#define N 100 
#define MIN_VALUE -10
#define MAX_VALUE 10

typedef struct {
    int vetor[N];
    int soma_parcial;
} shared_t;

void preencherVetor(shared_t* shared, int start, int end) {
    printf("Processo %d: Preenchendo vetor do índice %d até o índice %d\n", getpid(), start, end - 1);
    for (int i = start; i < end; i++) {
        shared->vetor[i] = (rand() % (MAX_VALUE - MIN_VALUE + 1)) + MIN_VALUE;
    }
}

void somarVetor(shared_t* shared, int start, int end) {
    printf("Processo %d: Somando vetor do índice %d até o índice %d\n", getpid(), start, end - 1);
    int soma_local = 0;
    for (int i = start; i < end; i++) {
        soma_local += shared->vetor[i];
    }
    shared->soma_parcial = soma_local;
}

int main() {
    printf("Iniciando processo principal\n");
    // Inicializar o gerador de números aleatórios
    srand(time(NULL));
    
    // Configurar memória compartilhada
    const int SIZE = sizeof(shared_t);
    const char *name = "OS";
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    shared_t* shared = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Preencher o vetor parcialmente no processo pai
    preencherVetor(shared, 0, N);

    int num_processos = 4; // Número total de processos desejados
    int processos_preenchimento = num_processos / 2;
    int processos_soma = num_processos - processos_preenchimento;

    int elementos_por_processo = N / processos_preenchimento;

    // Criar processos para preencher o vetor
    for (int i = 0; i < processos_preenchimento; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            // Erro ao criar processo
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Processo filho
            int start = i * elementos_por_processo;
            int end = (i + 1) * elementos_por_processo;
            preencherVetor(shared, start, end);
            exit(EXIT_SUCCESS);
        }
    }

    // Criar processos para somar o vetor
    for (int i = 0; i < processos_soma; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            // Erro ao criar processo
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Processo filho
            int start = i * elementos_por_processo;
            int end = (i + 1) * elementos_por_processo;
            somarVetor(shared, start, end);
            exit(EXIT_SUCCESS);
        }
    }

    // Esperar pelos processos filhos terminarem
    for (int i = 0; i < num_processos; i++) {
        int status;
        pid_t terminated_pid = wait(&status);
        if (terminated_pid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    // Somar as somas parciais dos processos filhos
    int soma_total = 0;
    for (int i = 0; i < num_processos; i++) {
        soma_total += shared->soma_parcial;
    }

    // Imprimir resultado
    printf("Processo %d: Soma agregada: %d\n", getpid(), soma_total);

    // Desalocar memória compartilhada
    shm_unlink(name);

    return 0;
}

// ---------------------- IMPORTANTE ----------------------
// Grupo 2 - Brian Medeiros, Mariana Meirelles e Nayara Gomes
// SO utilizado no desenvolvimento - MacOS
// IDE utilizada no desenvolvimento - VsCode

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

// Número de threads para cada elemento
int N_ELVES = 0;
static const int N_REINDEER = 9;

int elvesWaiting = 0;
int reindeerWaiting = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elveMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elveCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santaCond = PTHREAD_COND_INITIALIZER;

pthread_t *createThread(void *(*f)(void *), void *a)
{
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, f, a);
    return thread;
}

void *santaClausFunc()
{

    while (1)
    {
        pthread_mutex_lock(&santaMutex);            // Bloqueando o mutex do Papai Noel
        pthread_cond_wait(&santaCond, &santaMutex); // Esperando o sinal para o Papai Noel acordar
        pthread_mutex_lock(&mutex);                 // Bloqueando o Mutex

        //  Como preparar as renas para sair é a prioridade, ela é a primeira condição a ser verificada
        if (reindeerWaiting == N_REINDEER)
        {
            prepareSleigh();
            while (reindeerWaiting > 0)
            {
                pthread_cond_signal(&reindeerCond); // Preparando cada rena para depois serem liberadas
            }
            getHitched();
            reindeerWaiting = 0; // Zerando o n˚ de renas que estão esperando
        }
        else if (elvesWaiting == 3)
        {
            helpElves();
            pthread_cond_signal(&elveCond); // Enviando sinal para elfos que serão ajudados
        }

        pthread_mutex_unlock(&santaMutex); // Desbloqueando o mutex do Papai Noel
        pthread_mutex_unlock(&mutex);      // Desbloqueando o mutex
    }
}

void *reindeerFunc(void *arg)
{
    int id = (int)arg;
    printf("A rena %d chegou e está esperando o Papai Noel\n", id);

    pthread_mutex_lock(&mutex); // Bloqueando o mutex

    while (1)
    {
        reindeerWaiting++; // Acrescenta uma rena na lista de renas esperando

        if (reindeerWaiting == N_REINDEER) // Verifica se a quantidade de renas que estão esperando é igual ao n˚ mínimo de renas pro Papai Noel acordar
        {
            pthread_cond_signal(&santaCond); // Envia sinal pro Papai Noel ser acordado
        }
        pthread_mutex_unlock(&mutex); // Desbloqueando o mutex

        pthread_cond_wait(&reindeerCond, &reindeerMutex); // Esperando o Papai Noel sinalizar
    }
    pthread_exit(NULL); // Finaliza a thread
}

void *elveFunc(void *arg)
{

    int id = (int)arg;

    while (1)
    {
        bool need_help = random() % 100 < N_ELVES; // A ideia dessa variável na condicional é só simular qual elfo precisa de ajuda.
        if (need_help)
        {
            pthread_mutex_lock(&elveMutex); // Bloqueando o mutex dos elfos
            pthread_mutex_lock(&mutex);     // Bloqueando o mutex

            printf("O elfo %d precisa da ajuda do Papai Noel\n", id);

            elvesWaiting++; // Acrescenta um elfo na lista de elfos esperando

            if (elvesWaiting == 3)
            {
                printf("Chamando Papai Noel para ajudar os três elfos.\n");

                pthread_cond_signal(&santaCond); // Chamando o Papai Noel
            }

            else
            {
                pthread_mutex_unlock(&elveMutex); // Desbloqueando mutex caso outros elfos precisem de ajuda
            }

            pthread_cond_wait(&elveCond, &elveMutex); // Esperando o Papai Noel sinalizar

            elvesWaiting--;

            pthread_cond_signal(&elveCond); // Sinalizando que outro elfo pode receber ajuda, caso necessite

            if (elvesWaiting == 0)
            {
                pthread_mutex_unlock(&elveMutex); // Para evitar que hajam mais de três elfos sendo ajudados, desbloqueamos o mutex dos elfos apenas quando todos já receberam ajuda
            }
            pthread_mutex_unlock(&mutex); // Desbloqueando o mutex
        }
    }

    pthread_exit(NULL); // Finaliza a thread
}

// Chamadas para indicar ações de elfos, renas e papai noel
void prepareSleigh() { printf("Preparando o trenó!"); }
void helpElves() { printf("Ajudando elfos!"); }
void getHitched() { printf("Saindo com as renas!"); }
void getHelp() { printf("Pedindo ajuda ao Papai Noel!"); }

int main()
{

    printf("Quantos elfos trabalham para o Papai Noel? ");
    scanf("%d", &N_ELVES);

    elvesWaiting = 0;
    reindeerWaiting = 0;

    // Inicializando todos os mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&santaMutex, NULL);
    pthread_mutex_init(&reindeerMutex, NULL);
    pthread_mutex_init(&elveMutex, NULL);

    // Inicializando todas as variáveis de condição
    pthread_cond_init(&reindeerCond, NULL);
    pthread_cond_init(&elveCond, NULL);
    pthread_cond_init(&santaCond, NULL);

    // Criando a thread do Papai Noel
    pthread_t *santa_claus = CreateThread(santaClausFunc, 0);

    // Criando as threads das renas
    pthread_t *reindeers[N_REINDEER];
    for (int i = 0; i < N_REINDEER; i++)
    {
        reindeers[i] = CreateThread(reindeerFunc, (void *)i + 1);
    }

    // Criando as threads dos elfos
    pthread_t *elves[N_ELVES];
    for (int i = 0; i < N_ELVES; i++)
    {
        elves[i] = CreateThread(elveFunc, (void *)i + 1);
    }

    // Esperando a thread do Papai Noel terminar
    pthread_join(*santa_claus, NULL);

    // Esperando as threads das renas terminarem
    for (int i = 0; i < N_REINDEER; i++)
    {
        pthread_join(*reindeers[i], NULL);
    }

    // Esperando as threads das renas terminarem
    for (int i = 0; i < N_ELVES; i++)
    {
        pthread_join(*elves[i], NULL);
    }
}
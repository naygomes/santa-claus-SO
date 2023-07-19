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
int N_REINDEER = 9;

int elvesWaiting;
int reindeerWaiting;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elveMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elveCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santaCond = PTHREAD_COND_INITIALIZER;

pthread_t *createThread(void *(*function)(void *))
{
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, function, NULL);
    return thread;
}

// Chamadas para indicar ações de elfos, renas e papai noel
void prepareSleigh() { printf("Preparando o trenó!\n"); }
void helpElves() { printf("Ajudando elfos!\n"); }
void getHitched() { printf("Saindo com as renas!\n"); }
void getHelp() { printf("Pedindo ajuda ao Papai Noel!\n"); }

void *reindeerFunc()
{
    pthread_mutex_lock(&mutex); // Bloqueando o mutex
    // pthread_mutex_lock(&reindeerMutex); // Bloqueando o mutex das renas //-------------------

    reindeerWaiting++; // Acrescenta uma rena na lista de renas esperando

    printf("%d renas chegaram e estão esperando o Papai Noel\n", reindeerWaiting);

    if (reindeerWaiting == N_REINDEER) // Verifica se a quantidade de renas que estão esperando é igual ao n˚ mínimo de renas pro Papai Noel acordar
    {
        pthread_cond_signal(&santaCond); // Envia sinal pro Papai Noel ser acordado
    }
    pthread_mutex_unlock(&mutex);       //-------------------
    pthread_mutex_lock(&reindeerMutex); // Bloqueia o mutex das renas //-------------------

    pthread_cond_wait(&reindeerCond, &reindeerMutex); // Esperando o Papai Noel sinalizar

    pthread_mutex_unlock(&reindeerMutex); // Desbloqueando o mutex

    pthread_cond_signal(&reindeerCond); // Desbloqueando o mutex das renas

    pthread_exit(NULL); // Finaliza a thread
}

void *elveFunc()
{

    pthread_mutex_lock(&elveMutex); // Bloqueando o mutex dos elfos
    pthread_mutex_lock(&mutex);     // Bloqueando o mutex

    elvesWaiting++; // Acrescenta um elfo na lista de elfos esperando

    if (elvesWaiting <= 3)
        printf("%d elfos precisam da ajuda do Papai Noel\n", elvesWaiting);

    if (elvesWaiting == 3)
    {
        getHelp();

        pthread_cond_signal(&santaCond); // Chamando o Papai Noel
    }
    else if (elvesWaiting > 3)
    {
        printf("Já tem três elfos precisando de ajuda. Aguarde algum deles ser ajudado.\n");
        pthread_mutex_unlock(&elveMutex);
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }

    else
    {
        pthread_mutex_unlock(&elveMutex); // Desbloqueando mutex caso outros elfos precisem de ajuda
    }

    pthread_cond_wait(&elveCond, &mutex); // Esperando o Papai Noel sinalizar //-----------------------

    // Caso o papai noel não tenha sinalizado para os elfos ainda mas todas as renas tenham chegado, priorizamos a saída das renas
    if (reindeerWaiting == N_REINDEER)
    {
        pthread_mutex_unlock(&mutex);
        pthread_mutex_unlock(&elveMutex);
        pthread_cond_signal(&elveCond);
        pthread_exit(NULL);
    }

    printf("Um elfo já conseguiu ajuda\n");

    pthread_cond_signal(&elveCond); // Sinalizando que outro elfo pode receber ajuda, caso necessite

    elvesWaiting--;

    if (elvesWaiting == 0)
    {
        pthread_mutex_unlock(&elveMutex); // Para evitar que hajam mais de três elfos sendo ajudados, desbloqueamos o mutex dos elfos apenas quando todos já receberam ajuda
    }
    pthread_mutex_unlock(&mutex); // Desbloqueando o mutex

    pthread_exit(NULL); // Finaliza a thread
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
            pthread_cond_signal(&reindeerCond); // Preparando cada rena para depois serem liberadas
            pthread_mutex_unlock(&mutex);       // -----------
            pthread_cond_signal(&elveCond);     // -------------

            getHitched();

            pthread_exit(NULL); // ----------------
        }
        else if (elvesWaiting == 3)
        {
            helpElves();
            pthread_cond_signal(&elveCond); // Enviando sinal para elfos que serão ajudados
        }

        pthread_mutex_unlock(&mutex); // Desbloqueando o mutex

        pthread_mutex_unlock(&santaMutex); // Desbloqueando o mutex do Papai Noel
    }
}

int main()
{

    printf("Quantos elfos irão trabalhar para o Papai Noel? ");
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

    // Declarando Threads
    pthread_t santaClausThread;
    pthread_t reindeersThread[N_REINDEER];
    pthread_t elvesThread[N_ELVES];

    // Criando a thread do Papai Noel
    pthread_create(&santaClausThread, NULL, santaClausFunc, NULL);
    ;

    // Criando as threads das renas
    for (int i = 0; i < N_REINDEER; i++)
    {
        pthread_create(&reindeersThread[i], NULL, reindeerFunc, NULL);
    }

    // Criando as threads dos elfos
    for (int i = 0; i < N_ELVES; i++)
    {
        pthread_create(&elvesThread[i], NULL, elveFunc, NULL);
    }

    // Esperando a thread do Papai Noel terminar
    pthread_join(santaClausThread, NULL);

    // Esperando as threads das renas terminarem
    for (int i = 0; i < N_REINDEER; i++)
    {
        pthread_join(reindeersThread[i], NULL);
    }

    // Esperando as threads das renas terminarem
    for (int i = 0; i < N_ELVES; i++)
    {
        pthread_join(elvesThread[i], NULL);
    }

    // Destruindo mutex e variáveis de condição
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&santaMutex);
    pthread_mutex_destroy(&reindeerMutex);
    pthread_mutex_destroy(&elveMutex);

    pthread_cond_destroy(&reindeerCond);
    pthread_cond_destroy(&elveCond);
    pthread_cond_destroy(&santaCond);

    printf("Papai Noel terminou seu trabalho!\n");
    return 0;
}
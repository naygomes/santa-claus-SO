#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

int elves;
int reindeer;
sem_t santaSem;
sem_t reindeerSem;
sem_t elfTex;
sem_t mutex;

void santaFunc()
{
    sem_wait(&santaSem);
    sem_wait(&mutex);

    if (reindeer >= 9)
    {
        prepareSleigh();
        sem_post(&reindeerSem);
        reindeer -= 9;
    }
    else if (elves == 3)
    {
        helpElves();
        mutex_post();
    }
}

void reindeersFunc()
{
    sem_wait(&mutex);
    reindeer += 1;

    if (reindeer == 9)
    {
        sem_post(&santaSem);
    }

    sem_post(mutex);
    sem_wait(&reindeerSem);
    getHitched();
}

void elvesFunc()
{
    sem_wait(&elfTex);
    sem_wait(&mutex);
    elves += 1;

    if (elves == 3)
    {
        sem_post(&santaSem);
    }
    else
    {
        sem_post(&elfTex);
    }

    sem_post(&mutex);

    getHelp();
    sem_wait(&mutex);
    elves -= 1;

    if (elves == 0)
    {
        sem_post(&elfTex);
    }

    sem_post(&mutex);
}

void prepareSleigh() { printf("Preparando o trenó!"); }
void helpElves() { printf("Ajudando elfos!"); }
void getHitched() { printf("Vamo que vamo!"); }
void getHelp() { printf("Pedindo ajuda ao Papai Noel!"); }

int main()
{
    elves = 0;
    reindeer = 0;

    // ######### Analisar valores limites #########
    sem_init(&santaSem, 0, 1);
    sem_init(&reindeerSem, 0, 9);
    sem_init(&elfTex, 0, 1);
    sem_init(&mutex, 0, 1);
}
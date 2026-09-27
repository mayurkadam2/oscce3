#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10

int buffer[BUFFER_SIZE];

int in = 0;
int out = 0;

sem_t empty;
sem_t full;
sem_t mutex;

void *producer(void *arg)
{
    for (int item = 1; item <= TOTAL_ITEMS; item++)
    {
        // Wait for an empty slot
        sem_wait(&empty);

        // Enter critical section
        sem_wait(&mutex);

        // Put item into buffer
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;

        printf("Producer produced: %d\n", item);

        // Leave critical section
        sem_post(&mutex);

        // One more item is available
        sem_post(&full);
    }

    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 0; i < TOTAL_ITEMS; i++)
    {
        // Wait for an available item
        sem_wait(&full);

        // Enter critical section
        sem_wait(&mutex);

        // Take item from buffer
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        printf("Consumer consumed: %d\n", item);

        // Leave critical section
        sem_post(&mutex);

        // One more empty slot is available
        sem_post(&empty);
    }

    return NULL;
}

int main()
{
    pthread_t producer_thread;
    pthread_t consumer_thread;

    // Initial semaphore values
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    // Create threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait for both threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Destroy semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}
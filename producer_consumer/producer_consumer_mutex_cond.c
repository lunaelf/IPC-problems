#include <stdio.h>
#include <pthread.h>

#define MAX 1000 // how many numbers to produce
#define N 100

pthread_mutex_t mutex;
pthread_cond_t condc, condp; // used for signaling

int buffer[N]; // buffer used between producer and consumer
int count = 0; // how many items in the buffer
int lo = 0;    // pointer for consumer
int hi = 0;    // pointer for producer

void *producer(void *ptr); // produce data
void *consumer(void *ptr); // consume data

int main(int argc, char **argv)
{
    pthread_t pro, con;

    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&condc, 0);
    pthread_cond_init(&condp, 0);

    pthread_create(&con, 0, consumer, 0);
    pthread_create(&pro, 0, producer, 0);
    pthread_join(pro, 0);
    pthread_join(con, 0);

    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&mutex);

    return 0;
}

/* produce data */
void *producer(void *ptr)
{
    int i;

    for (i = 1; i <= MAX; i++)
    {
        printf("produce: %d\n", i);

        pthread_mutex_lock(&mutex); // get exclusive access to buffer
        while (count == N)
        {
            pthread_cond_wait(&condp, &mutex);
        }
        buffer[hi] = i; // put item in buffer
        hi = (hi + 1) % N;
        count = count + 1;
        // if consumer was sleeping, wake it up
        if (count == 1)
        {
            pthread_cond_signal(&condc); // wake up consumer
        }
        pthread_mutex_unlock(&mutex); // release access to buffer
    }

    pthread_exit(0);
}

/* consume data */
void *consumer(void *ptr)
{
    int i;
    int item;

    for (i = 1; i <= MAX; i++)
    {
        pthread_mutex_lock(&mutex); // get exclusive access to buffer
        while (count == 0)
        {
            pthread_cond_wait(&condc, &mutex);
        }
        item = buffer[lo]; // take item out of buffer
        lo = (lo + 1) % N;
        count = count - 1;
        // if producer was sleeping, wake it up
        if (count == N - 1)
        {
            pthread_cond_signal(&condp); // wake up producer
        }
        pthread_mutex_unlock(&mutex); // release access to buffer

        printf("consume: %d\n", item);
    }

    pthread_exit(0);
}

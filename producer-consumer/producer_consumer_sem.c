#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 10000 // how many numbers to produce
#define N 100     // number of slots in the buffer

sem_t *mutex; // controls access to critical region
sem_t *empty; // counts empty buffer slots
sem_t *full;  // counts full buffer slots

int buffer[N];  // buffer used between producer and consumer
int count = 0;  // how many items in the buffer
int lo = 0;     // pointer for consumer
int hi = 0;     // pointer for producer
int i_item = 1; // helper for producing item

void *producer(void *ptr);
void *consumer(void *ptr);

int produce_item();
void insert_item(int item);
int remove_item();
void consume_item(int item);

int main(int argc, char **argv)
{
    pthread_t pro, con;

    mutex = sem_open("/mutex", O_CREAT, 0644, 1);
    empty = sem_open("/empty", O_CREAT, 0644, N);
    full = sem_open("/full", O_CREAT, 0644, 0);

    pthread_create(&con, 0, consumer, 0);
    pthread_create(&pro, 0, producer, 0);
    pthread_join(pro, 0);
    pthread_join(con, 0);

    sem_close(full);
    sem_close(empty);
    sem_close(mutex);

    return 0;
}

void *producer(void *ptr)
{
    int i;
    int item;

    for (i = 1; i <= MAX; i++)
    {
        item = produce_item(); // generate something to put in buffer
        sem_wait(empty);       // decrement empty count
        sem_wait(mutex);       // enter critical region
        insert_item(item);     // put new item in buffer
        sem_post(mutex);       // leave critical region
        sem_post(full);        // increment count of full slots
    }

    pthread_exit(0);
}

void *consumer(void *ptr)
{
    int i;
    int item;

    for (i = 1; i <= MAX; i++)
    {
        sem_wait(full);       // decrement full count
        sem_wait(mutex);      // enter critical region
        item = remove_item(); // take item from buffer
        sem_post(mutex);      // leave critical region
        sem_post(empty);      // increment count of empty slots
        consume_item(item);   // do something with the item
    }

    pthread_exit(0);
}

int produce_item()
{
    int item = i_item;
    i_item++;
    printf("produce: %d\n", item);
    return item;
}

void insert_item(int item)
{
    buffer[hi] = item; // put item in buffer
    hi = (hi + 1) % N;
    count = count + 1;
}

int remove_item()
{
    int item = buffer[lo]; // take item out of buffer
    lo = (lo + 1) % N;
    count = count - 1;
    return item;
}

void consume_item(int item)
{
    printf("consume: %d\n", item);
}

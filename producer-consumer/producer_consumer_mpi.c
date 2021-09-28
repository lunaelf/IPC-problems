#include <stdio.h>
#include <mpi.h>

#define MAX 10000 // how many numbers to produce
#define N 100     // number of slots in the buffer

int i_item = 1; // helper for producing item

int produce_item();
void build_message(int *message, int item);
int extract_item(int *message);
void consume_item(int item);

int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming 2 processes for this task
    if (world_size != 2)
    {
        fprintf(stderr, "World size must be two for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int other = (world_rank + 1) % world_size;
    if (world_rank == 0) // producer process
    {
        int item;
        int msg; // message buffer
        int i;

        for (i = 0; i < MAX; i++)
        {
            item = produce_item();                                                   // generate something to put in buffer
            MPI_Recv(&msg, 1, MPI_INT, other, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // wait for an empty to arrive
            build_message(&msg, item);                                               // construct a message to send
            MPI_Send(&msg, 1, MPI_INT, other, 0, MPI_COMM_WORLD);                    // send item to sonsumer
        }
    }
    else if (world_rank == 1) // consumer process
    {
        int item;
        int msg;
        int i, j;

        for (i = 0; i < N; i++)
        {
            MPI_Send(&msg, 1, MPI_INT, other, 0, MPI_COMM_WORLD); // send N empties
        }
        for (j = 0; j < MAX; j++)
        {
            MPI_Recv(&msg, 1, MPI_INT, other, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // get message containing item
            item = extract_item(&msg);                                               // extract item from message
            MPI_Send(&msg, 1, MPI_INT, other, 0, MPI_COMM_WORLD);                    // send back empty reply
            consume_item(item);                                                      // do something with the item
        }
    }

    MPI_Finalize();

    return 0;
}

int produce_item()
{
    int item = i_item;
    i_item++;
    printf("produce: %d\n", item);
    return item;
}

void build_message(int *message, int item)
{
    *message = item;
}

int extract_item(int *message)
{
    return *message;
}

void consume_item(int item)
{
    printf("consume: %d\n", item);
}

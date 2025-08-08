#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_SIZE sizeof(Node_t) * 10
#define SEM_KEY 1234
#define SHM_KEY 5678

typedef struct Node
{
    int data;
    struct Node *next;
} Node_t;

void create_shm(void)
{
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);

    if (shm_id == -1)
    {
        perror("shm failed");
        exit(EXIT_FAILURE);
    }

    // shmat, shared memory attach - retrieve a point to already allocated shared memory.
    // NULL == give me any address; 0 == no flags.
    Node_t *shm_ptr = (Node_t *)shmat(shm_id, NULL, 0);
    if(shm_ptr == (void *)-1)
    {
        perror("shm_ptr failed");
        exit(EXIT_FAILURE);
    }

    int sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1)
    {
        perror("sem_id failed");
        exit(EXIT_FAILURE);
    }

    semctl(sem_id, 0, SETVAL, 1);

    Node_t *head = shm_ptr;

    for (int i = 0; i < 10; i++)
    {
        head[i].data = i;
        head[i].next = i < 9 ? &head[i+1] : NULL; 
    }

    printf("SHM and list created.\n");

    // detach share memory
    shmdt(shm_ptr);
    printf("Detached from shared memory.\n");
}

void get_shm(void)
{
    int shm_id = shmget(SHM_KEY, 0, 0666);
    if (shm_id == -1)
    {
        perror("shm failed");
        exit(EXIT_FAILURE);
    }

    // shmat, shared memory attach - retrieve a point to already allocated shared memory.
    // NULL == give me any address; 0 == no flags.
    Node_t *shm_ptr = (Node_t *)shmat(shm_id, NULL, 0);
    if(shm_ptr == (void *)-1)
    {
        perror("shm_ptr failed");
        exit(EXIT_FAILURE);
    }

    int sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1)
    {
        perror("sem_id failed");
        exit(EXIT_FAILURE);
    }

    struct sembuf sop =
    {
        .sem_num = 0,
        .sem_op = -1, // lock
        .sem_flg = 0
    };

    // operate on semaphore
    semop(sem_id, &sop, 1);

    Node_t *current = shm_ptr;
    int counter = 0;

    while(current != NULL)
    {
        printf("Node %d contains the value %d\n", counter, current->data);
        counter++;
        current = current->next;
    }

    // release lock
    sop.sem_op = 1;
    semop(sem_id, &sop, 1);

    // delete shared memory
    shmctl(SHM_KEY, IPC_RMID, NULL);
    printf("Deleted shared memory.\n");
}

int main(void)
{
    int fork_ret = fork();

    if (fork_ret == 0)
    {
        create_shm();
    }
    else
    {
        sleep(1);
        get_shm();
    }

    return (EXIT_SUCCESS);
}

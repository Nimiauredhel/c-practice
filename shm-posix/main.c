#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include<semaphore.h>

#define SEM_NAME "CSCRATCH_POSIX_SEM"
#define SHM_NAME "CSCRATCH_POSIX_SHM"
#define SHM_SIZE (sizeof(ExampleStruct_t))
#define CONTENTS_MAX_LEN (24)
#define COUNTER_MAX (10)

typedef struct ExampleStruct
{
    uint64_t write_count;
    char contents[CONTENTS_MAX_LEN];
    char pad[4];
} ExampleStruct_t;

static const char *out_msgs[COUNTER_MAX] =
{
    "Zero", "One", "Two", "Three", "Four",
    "Five", "Six", "Seven", "Eight", "Nine",
};

static void process(bool even)
{
    char msg_buff[CONTENTS_MAX_LEN+1] = {0};
    uint64_t counter = 0;

    char *procname = even ? "EVEN" : "ODD";

    sem_t *sem = sem_open(SEM_NAME, O_RDWR);

    if (sem == NULL)
    {
        perror("Failed to open sem");
        exit(EXIT_FAILURE);
    }

    printf("%s opened sem.\n", procname);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); 

    if (shm_fd <= 0)
    {
        perror("Failed to open shm");
        exit(EXIT_FAILURE);
    }

    printf("%s opened shm.\n", procname);

    void *shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    ExampleStruct_t *cast_ptr = (ExampleStruct_t *)shm_ptr;

    if (shm_ptr == NULL)
    {
        perror("Failed to mmap shm");
        exit(EXIT_FAILURE);
    }

    printf("%s mapped shm.\n", procname);

    bool my_turn = false;

    while(counter < COUNTER_MAX)
    {
        usleep(even ? 128000 : 64000);
        sem_wait(sem);
        printf("%s acquired sem.\n", procname);

        counter = cast_ptr->write_count;
        strncpy(msg_buff, cast_ptr->contents, CONTENTS_MAX_LEN);

        printf("%s read counter: %lu.\n", procname, counter);
        printf("%s read contents: %s.\n", procname, msg_buff);

        my_turn = even == (counter % 2 == 0);

        if (my_turn)
        {
            counter++;
            cast_ptr->write_count = counter;

            if (counter < COUNTER_MAX)
            {
                printf("%s writing [%s] to contents.\n", procname, out_msgs[counter]);
                strncpy(cast_ptr->contents, out_msgs[counter], CONTENTS_MAX_LEN);
            }
            else
            {
                printf("%s hit max counter.\n", procname);
            }
        }

        printf("%s releasing sem.\n", procname);
        sem_post(sem);
    }
}

static void init(void)
{
    sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 0);

    if (sem == NULL)
    {
        perror("Failed to open sem");
        exit(EXIT_FAILURE);
    }

    printf("Init sem.\n");
    sem_post(sem);
    sem_close(sem);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); 
    ftruncate(shm_fd, SHM_SIZE);
    void *shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memset(shm_ptr, 0, SHM_SIZE);
    snprintf(((ExampleStruct_t *)shm_ptr)->contents, CONTENTS_MAX_LEN, "Initial");
}

int main(void)
{
    init();

    int fork_ret = fork();

    if (fork_ret == 0)
    {
        process(false);
    }
    else
    {
        process(true);
    }

    return (EXIT_SUCCESS);
}

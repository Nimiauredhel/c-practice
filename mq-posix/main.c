#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <mqueue.h>

#define QUEUE_NAME "/c_scratch_test_queue"
#define MSG_MAX_SIZE (32)
#define MSG_MAX_COUNT (10)
#define MSG_STOP "exit"

static struct mq_attr attr = 
{
    .mq_flags = 0,
    .mq_maxmsg = MSG_MAX_COUNT,
    .mq_msgsize = MSG_MAX_SIZE,
    .mq_curmsgs = 0
};

static void receiver(void)
{
    char msg_buff[MSG_MAX_SIZE+1] = {0};

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);

    if (mq == -1)
    {
        perror("Receiver failed to open mq");
        exit(EXIT_FAILURE);
    }

    printf("Receiver opened mq.\n");

    for(;;)
    {
        memset(msg_buff, '\0', sizeof(msg_buff));

        ssize_t bytes_read = mq_receive(mq, msg_buff, MSG_MAX_SIZE, NULL);

        if (bytes_read < 0)
        {
            perror("Receiver failed to receive msg, quitting");
            mq_close(mq);
            exit(EXIT_FAILURE);
        }

        if (!strncmp(msg_buff, MSG_STOP, strlen(MSG_STOP)))
        {
            printf("Receiver received exit msg, quitting.\n");
            break;
        }

        printf("Receiver received msg: [%s].\n", msg_buff);
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
}

static void sender(void)
{
#define SENDER_NUM_MSGS (10)

    static const char *out_msgs[SENDER_NUM_MSGS] =
    {
        "Zero", "One", "Two", "Three", "Four",
        "Five", "Six", "Seven", "Eight", "Nine",
    };

    sleep(1);
    mqd_t mq = mq_open(QUEUE_NAME, O_WRONLY);

    if (mq == -1)
    {
        perror("Sender failed to open mq");
        exit(EXIT_FAILURE);
    }

    printf("Sender opened mq.\n");

    for (int i = 0; i < SENDER_NUM_MSGS; i++)
    {
        printf("Sender sending msg: [%s].\n", out_msgs[i]);
        mq_send(mq, out_msgs[i], strlen(out_msgs[i])+1, 0);
    }

    printf("Sender sending exit msg.\n");
    mq_send(mq, MSG_STOP, strlen(MSG_STOP)+1, 0);
}

int main(void)
{
    int fork_ret = fork();

    if (fork_ret == 0)
    {
        sender();
    }
    else
    {
        receiver();
    }

    return (EXIT_SUCCESS);
}

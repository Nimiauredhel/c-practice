#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t self_pid = -1;
pid_t child_pid = -1;

void parent(void)
{
    printf("Parent process created child with PID %d, waiting for its termination.\n", child_pid);
    waitpid(child_pid, NULL, 0);
    printf("Parent process reaped child with PID %d. Terminating.\n", child_pid);
}

void child(void)
{
    self_pid = getpid();
    printf("Child process %d started, sleeping for 3 seconds...\n", self_pid);
    sleep(3);
    printf("Child process %d back from sleep, terminating.\n", self_pid);
}

int main(void)
{
    self_pid = getpid();
    printf("Program started with PID %d. Forking...\n", self_pid);

    child_pid = fork();

    if (child_pid == 0)
    {
        child();
    }
    else
    {
        parent();
    }
}

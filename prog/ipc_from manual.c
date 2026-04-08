/*
create a c program to demonstrate the ipc using shared memory, msg queue, pipes.
The program should include functions to create and use shared memory, msg queues and pipes to exchange data between processes.
  */


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<sys/wait.h>

#define SIZE 1024

// Message Queue structure
struct msgbuf {
    long msgtype;
    char data[100];
};

int main(){

    int pipefd[2];
    int shmid;
    char *shm_ptr;
    key_t key;
    int mqid;

    struct msgbuf msg;

    // Create Pipe
    pipe(pipefd);

    // Create Shared Memory
    key = ftok("file.txt", 65);
    shmid = shmget(key, SIZE, 0666 | IPC_CREAT);
    shm_ptr = (char*)shmat(shmid, NULL, 0);

    // Create Message Queue
    mqid = msgget(key, 0666 | IPC_CREAT);

    pid_t pid = fork();

    if(pid == 0){
        // ================= CHILD PROCESS =================

        char pipe_msg[100];

        // PIPE (Read)
        close(pipefd[1]);
        read(pipefd[0], pipe_msg, sizeof(pipe_msg));
        printf("Child received from PIPE: %s\n", pipe_msg);
        close(pipefd[0]);

        // SHARED MEMORY (Read)
        printf("Child read from SHARED MEMORY: %s\n", shm_ptr);

        // MESSAGE QUEUE (Send back to parent)
        msg.msgtype = 1;
        strcpy(msg.data, "Hello Parent, Message Queue here!");
        msgsnd(mqid, &msg, sizeof(msg.data), 0);

        // Detach shared memory
        shmdt(shm_ptr);

        exit(0);
    }
    else{
        // ================= PARENT PROCESS =================

        // PIPE (Write)
        close(pipefd[0]);
        write(pipefd[1], "Hello Child from PIPE", 25);
        close(pipefd[1]);

        // SHARED MEMORY (Write)
        strcpy(shm_ptr, "Hello Child from SHARED MEMORY");

        // Wait for child
        wait(NULL);

        // MESSAGE QUEUE (Receive)
        msgrcv(mqid, &msg, sizeof(msg.data), 1, 0);
        printf("Parent received from MESSAGE QUEUE: %s\n", msg.data);

        // Cleanup
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, NULL);
        msgctl(mqid, IPC_RMID, NULL);

        printf("Resources cleaned up.\n");
    }

    return 0;
}

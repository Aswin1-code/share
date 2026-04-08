// question : 
/*
An embedded system continuously reads keycode from the alphanumeric keyboard. 
And the keycodes are stored in RAM. Suggest the suitable IPC kernel object to form above task and explain in detail.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SIZE 1024

int main(){
    key_t key;
    int shmid;
    char *shm_ptr;

    // Generate key
    key = ftok("file.txt", 65);

    // Create shared memory
    shmid = shmget(key, SIZE, 0666 | IPC_CREAT);

    // Attach shared memory
    shm_ptr = (char*)shmat(shmid, NULL, 0);

    pid_t pid = fork();

    if(pid == 0){
        // ================= CONSUMER =================
        while(1){
            printf("Consumer read keycode: %s\n", shm_ptr);
            sleep(2);
        }
    }
    else{
        // ================= PRODUCER =================
        char input[100];

        while(1){
            printf("Enter keycode: ");
            scanf("%s", input);

            strcpy(shm_ptr, input);
        }
    }

    // Cleanup (never reached in loop, but needed in exam)
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

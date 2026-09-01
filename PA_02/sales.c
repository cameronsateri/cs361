//---------------------------------------------
// Assignment  : PA-2 FSM
// Date        : 11/4/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : sales.c
//---------------------------------------------

// set up shared memory and initialize its objects
// set up the semaphores and the messages queue
// fork / execute supervisor process
// fork / execute all factory processes
// wait for supervisor to indicate that manufacturing is done
// give supervisor permission to print the final report
// clean up after zombie process (supervisor + all factories)
// destroy the shared memory
// destroy the semaphores and the message queue

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include "wrappers.h"
#include "message.h"
#include "shmem.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int msgid, shmid, fsid;
sem_t *sem;
shData *p ;
sem_t *mutex, *factorySem, *printSem, *factoryMutex, *shmMutex;

void abnormalTermination( int sig )
{
    fflush( stdout ) ;
    killpg(fsid, SIGKILL);
    
    Sem_close(factorySem);
    Sem_unlink("/factorySem");
    Sem_close(printSem);
    Sem_unlink("/printSem");
    Sem_close(factoryMutex);
    Sem_unlink("/factoryMutex");
    Sem_close(shmMutex);
    Sem_unlink("/shmMutex");

    Shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    msgctl(msgid, IPC_RMID, NULL);
    
    printf( "\nCleaned up and exiting.\n\n" );
    exit(0) ;
}

int main(int argc , char *argv[]){
    //Ensuring correct number of arguments
    if(argc != 3){
        printf("Incorrect number of arguments inputed");
        return EXIT_FAILURE;
    }

    int N = atoi(argv[1]); //Number of factory lines
    int order_size = atoi(argv[2]); //Total order size

    //Ensuring Number of facotry lines is valid
    if(N < 1 || N > MAXFACTORIES){
        printf("Error: Number of factories must be between 1 and %d, but is %d", MAXFACTORIES, N);
        return EXIT_FAILURE;
    }

    printf("SALES: Will Request an Order of Size = %3d parts\n", order_size);
    printf("Creating %d Factory(ies)\n", N);

    //Creating shared memory
    key_t shm_key = ftok("shmem.h", 5);
    if(shm_key < 0){
        printf("Error creating shm_key");
    }

    int shmflg = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR ; 
    shmid  = Shmget(shm_key, SHMEM_SIZE, shmflg);
    
    if(shmid == -1){
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }
   
    p = (shData *) Shmat(shmid, NULL, 0);
    if(p == (shData *) -1){
        perror("Failed to attach shared memory");
        exit(EXIT_FAILURE);
    }
    
    //Initializing shared memory data.
    p->order_size = order_size;
    p->made = 0;
    p->remain = order_size;
    p->activeFactories = N;

    //Create a messageQueue
    key_t msg_key = ftok("message.h", 7);
    if(msg_key == -1){
        perror("ftok failed\n");
        perror("This error is in sales.");
        exit(EXIT_FAILURE);
    }
    int msgflg = IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR;
    msgid = msgget(msg_key, msgflg);

    //Create semaphores.
    int semflg = O_CREAT | O_EXCL;
    int semmode = S_IRUSR | S_IWUSR;

    factorySem     = Sem_open("/lleshiax_satericj_factorySem"       , semflg, semmode, 0);//first two are between sales and supervisor, waits ot say it can print stuff
    printSem       = Sem_open("/lleshiax_satericj_printSem"         , semflg, semmode, 0);
    factoryMutex   = Sem_open("/lleshiax_satericj_factoryMutex"     , semflg, semmode, 1);
    shmMutex       = Sem_open("/lleshiax_satericj_shmMutex"         , semflg, semmode, 1);
    
    //Fork supervisor processes.
    char factories[15];
    snprintf(factories, sizeof(factories), "%d", N);
    
    int supervisor_pid = Fork();
    if(supervisor_pid == 0){
        int log_fd = open("supervisor.log", O_WRONLY | O_CREAT | O_TRUNC, 0600);
        fsid = getpid();
        dup2(log_fd, 1); // Redirect supervisor stdout to supervisor.log
        close(log_fd);

        if(execl("./supervisor", "supervisor", factories, NULL) == -1){
                perror("execl failed");
                exit(EXIT_FAILURE);
        }
    }

    srand(time(NULL));
    
    //Fork factory processes.
    int cap;
    int dur;
    char factStr[15];
    char capStr[15];
    char durStr[15];
    int log_fd = open("factory.log", O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0600);
    if(log_fd == -1){
        perror("unable to create factory log\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < N; i++){
        cap = (rand() % 41) + 10;
        dur = (rand() % 701) + 500;
        int factoryID = i + 1;
        pid_t factoryPid = Fork();
        if(factoryPid == 0){
            printf("SALES:  Factory # %d was created, with Capacity= %3d and Duration= %3d\n", factoryID, cap, dur);
            dup2(log_fd, 1);
            close(log_fd);

            snprintf(factStr, sizeof(factStr), "%d", factoryID);
            snprintf(capStr, sizeof(capStr), "%d", cap);
            snprintf(durStr, sizeof(durStr), "%d", dur);

            if(execl("./factory", "factory", factStr, capStr, durStr, NULL) == -1){//need 3 things here factoryID capicity (srand) and duration(rand)
                perror("factory execl failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    //Abnormal termination
    sigactionWrapper(SIGTERM, abnormalTermination);
    sigactionWrapper(SIGINT, abnormalTermination);

    Sem_wait(factorySem);
    printf("SALES: Supervisor says all Factories have completed their mission\n");

    //semaphore here instead, wait, then sleep
    sleep(2);

    Sem_post(printSem);

    printf("SALES: Permission granted to print final report\nSALES: Cleaning up after the Supervisor Factory Processes\n");

    Sem_close(factorySem);
    Sem_unlink("/lleshiax_satericj_factorySem");
    Sem_close(printSem);
    Sem_unlink("/lleshiax_satericj_printSem");
    Sem_close(factoryMutex);
    Sem_unlink("/lleshiax_satericj_factoryMutex");
    Sem_close(shmMutex);
    Sem_unlink("/lleshiax_satericj_shmMutex");

    Shmdt(p);
    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
}


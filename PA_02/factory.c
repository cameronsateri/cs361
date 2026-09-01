//---------------------------------------------
// Assignment  : PA-2 FSM
// Date        : 11/4/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : factory.c
//---------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "message.h"
#include "wrappers.h"
#include "shmem.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>


// #iterations = #partsMadeByMe = 0;
// While ( remain > 0 ) {
    // Determine how many to make = the smaller of ( remain , myCapacity).
    // Update remain
    // Print( “Factory #%3d: Going to make %5d parts in %4d milliSecs” , myID, data ... ) ;
    // sleep ( myDuration in milliseconds ) ; 
    // Same duration even if I make less than my capacity
    // Create & Send Production message to Supervisor 
    // Increment #iterations
    // Update my record of total #of parts I have made so far
// }
// Create & Send Completion message to Supervisor
// Print(“Factory Line %d: Completed after making total of %d parts in %d iterations” , myID, data ... );

int msgid;
int shmid;
sem_t *sem;
shData *p;
sem_t *factoryMutex, *shmMutex;

int main( int argc , char *argv[] )
{
    // Send to correct output
    // FILE *file;
    // file = open("factory.log", "w");
    //dup2(fileno(file), 1);
    //use dup2 to copy file pointer to standard out

    // Error check for incorrect input (probably wont happen)
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <factory_id> <capacity> <duration>\n", argv[0]);
        exit(1);
    }

    // Saves input from sales
    int factory_id = atoi(argv[1]);
    int capacity = atoi(argv[2]);
    int duration = atoi(argv[3]);

    // Receive shared memory info
    key_t shm_key = ftok("shmem.h", 5);

    int shmflg = IPC_CREAT | S_IRUSR | S_IWUSR ; 
    shmid  = Shmget(shm_key, SHMEM_SIZE, shmflg);

    if(shmid == -1){
        perror("Failed to get shared memory");
        exit(EXIT_FAILURE);
    }

    p = (shData *) Shmat(shmid, NULL, 0);
    if(p == (shData *) -1){
        perror("Failed to find shared memory");
        exit(EXIT_FAILURE);
    }

    // Receive message queue info
    int msgflg = S_IRUSR | S_IWUSR ; 
    key_t msg_key = ftok("message.h", 7);
    msgid = Msgget(msg_key, msgflg);

    //int semflg = O_CREAT | O_EXCL;
    //int semmode = S_IRUSR | S_IWUSR;

    sem_t *factoryMutex;
    sem_t *shmMutex;

    factoryMutex     = Sem_open2("/lleshiax_satericj_factoryMutex"      , 0);
    shmMutex         = Sem_open2("/lleshiax_satericj_shmMutex"      , 0);

    // Executes order
    int made = 0;
    int iterations = 0;

    Sem_wait( factoryMutex);
    printf("Factory # %2d: STARTED. My capacity = %3d, in %4d milliSeconds.\n", factory_id, capacity, duration);
    fflush(stdout);
    Sem_post( factoryMutex);
    
    Sem_wait(shmMutex);
    while (p->remain > 0)
    {
        int to_make;
        if(p->remain > capacity){
            to_make = capacity;
        }else{
            to_make = p->remain;
        }

        
        p->remain -= to_make;
        Sem_post(shmMutex);

        Sem_wait(factoryMutex);
        printf("Factory #%3d: Going to make %5d parts in %4d milliSecs\n", factory_id, to_make, duration);
        fflush(stdout);
        Sem_post(factoryMutex);

        // Simulate manufacturing process
        Usleep(duration * 1000);
        made += to_make;
        
        // Update shared memory
        Sem_wait(shmMutex);
        p->made += to_make;
        Sem_post(shmMutex);

        //make all the variables and message sen to the supervisors
        //make a message buf
        msgBuf message;
        message.mtype = 1;
        message.facID = factory_id;
        message.purpose = PRODUCTION_MSG;
        message.partsMade = to_make;
        message.duration = duration;

        msgsnd(msgid, &message, MSG_INFO_SIZE, 0);

        //made = p->made;
        iterations++;

        Sem_wait(shmMutex);
    }

    Sem_post(shmMutex);

    //Sem_wait(shmMutex);
    //made = p->made;
    //Sem_post(shmMutex);

    msgBuf commessage;
    commessage.mtype = 1;
    commessage.facID = factory_id;
    commessage.purpose = COMPLETION_MSG;
    commessage.partsMade = made;
    commessage.duration = duration;

    msgsnd(msgid, &commessage, MSG_INFO_SIZE, 0);


    Sem_wait(factoryMutex);
    printf(">>> Factory #   %d: Terminating after making total of     %d parts in     %d iterations\n" , factory_id, made, iterations);
    fflush(stdout);
    Sem_post(factoryMutex);
   
    // Close and unlink shared memory and semaphore
    shmdt(p) ;
    Sem_close(factoryMutex);
    Sem_close(shmMutex);

    return 0;
}


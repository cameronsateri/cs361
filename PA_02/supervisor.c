//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 11/4/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : supervisor.c
//---------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "message.h"
#include "wrappers.h"
#include "shmem.h"
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

// activeFactories= argv[1] ;
    // While ( activeFactories> 0 ) {
        // Receive a message from the messgaeQueue ;
        // If ( Production Message )
            // Print( “Factory xx produced nn parts in mm milliSecs” ) ;
            // Update per-factory production aggregates (num-parts-built, num-iterations) ;
        // Else if ( Termination Message )
            // Print( “Factory xx Terminated” ) ;
            // activeFactories-- ;
        // else
            // discard any unsupported message
    // }
    // Inform the Sales that manufacturing is done ;
    // Wait for permission from Sales to print final report ;
    // Print per-factory production aggregates sorted by Factory ID ;

int msgid;
int shmid;
sem_t *sem;
shData *p;
sem_t *printSem, *factorySem;

int main( int argc , char *argv[] )
{
    // Send to correct output
    // FILE *file;
    // file = open("supervisor.log", "w");
    //dup2(fileno(file), 1);

    if(argc != 2){
        printf("Usage: %s <activeFactories>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Saves inputs from sales
    int activeFactories = atoi(argv[1]);

    int productionStat[MAXFACTORIES][2];
    memset(productionStat, 0, sizeof(productionStat));
    
    // Receive shared memory info
    key_t shm_key = ftok("shmem.h", 5);

    int shmflg = S_IRUSR | S_IWUSR ; 
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
    key_t msg_key = ftok("message.h", 7);
    int msgflg = S_IRUSR | S_IWUSR;
    msgid = Msgget(msg_key, msgflg);
    if (msgid == -1) {
        perror("Failed to get Msg queue");
        exit(EXIT_FAILURE);
    }

    sem_t *printSem;
    sem_t *factorySem;

    printSem            = Sem_open2("/lleshiax_satericj_printSem"     , 0);
    factorySem          = Sem_open2("/lleshiax_satericj_factorySem"      , 0);

    msgBuf message;
    printf("SUPERVISOR: Started\n");
    fflush(stdout);

    while(activeFactories > 0){
        int bytesRec = msgrcv(msgid, &message, MSG_INFO_SIZE, 1, 0);

        if (bytesRec == -1) {
            perror("Failed to receive message");
            continue;
        }
        //printMsg(message);
        switch(message.purpose){
            case PRODUCTION_MSG:
                printf("SUPERVISOR: Factory %d produced %d parts in %d milliSecs\n", message.facID, message.partsMade, message.duration);
                fflush(stdout);

                productionStat[message.facID][0] += message.partsMade;
                productionStat[message.facID][1] += 1;
                break;
            case COMPLETION_MSG:
                printf("SUPERVISOR: Factory %d      COMPLETED its task\n", message.facID);
                fflush(stdout);

                activeFactories--;
                break;
            default:
                printf("Unsupported message type received\n");
                fflush(stdout);

                break;
        }
    }

    Sem_post(factorySem);
    Sem_wait(printSem);
    
    printf("\nSUPERVISOR: Manufacturing is complete. Awaiting permission to print final report\n");
    fflush(stdout);
    printf("\n****** SUPERVISOR: Final Report ******\n");
    fflush(stdout);

    int grandTotal = 0;
    for (int i = 0; i < MAXFACTORIES; i++) {
        if (productionStat[i][1] > 0) {
            printf("Factory # %d made a total of  %d parts in    %d iterations\n", 
                   i, productionStat[i][0], productionStat[i][1]);
            fflush(stdout);
            grandTotal += productionStat[i][0];
        }
    }
    printf("========================\n");
    fflush(stdout);
    printf("Grand total parts =    %d   vs   order size of   %d\n", grandTotal, p->order_size);
    fflush(stdout);
    
    // Close shared memory and semaphore
    printf("\n>>> Supervisor Terminated\n");
    shmdt(p) ;
    Sem_close(factorySem); 
    fflush(stdout);
    return 0;
}


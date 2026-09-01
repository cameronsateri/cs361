/*-------------------------------------------------------------------------------
IPC using Shared Memory & Signals Divider Version 00
Written By: 
     1- Dr. Mohamed Aboutabl
-------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include  <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "shmSegment.h"

int main()
{
    pid_t  mypid      = getpid() ;
    printf("\n\nHELLO! I am the newly-born DIVIDER with pid= %d\n\n" , mypid );

    key_t key = ftok("shmSegment.h", 65);
    int shmid = shmget(key, SHMEM_SIZE, 0666);
    shmData* shm_ptr = (shmData*)shmat(shmid, NULL, 0);

    if (shm_ptr->num2 != 0) {
        shm_ptr->ratio = (double)shm_ptr->num1 / shm_ptr->num2;
        printf("DIVIDER:        Dividing %d by     %d\n\n", shm_ptr->num1, shm_ptr->num2);
    }

    printf("DIVIDER: will Signal my parent to CONTINUE\n");
    kill(getppid(), SIGCONT);
    shmdt(shm_ptr);

    return 0;

}

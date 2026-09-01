/*-------------------------------------------------------------------------------
IPC using Shared Memory & Signals -- Counter Version 00
Written By: 
     1- Dr. Mohamed Aboutabl
-------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/shm.h>
#include "shmSegment.h"

bool FOREVER = true ;
int shmid; //Shared memory ID
shmData* shm_ptr;

//------------------------------------------------------------
/* Wrapper for sigaction */

typedef void Sigfunc( int ) ;

Sigfunc * sigactionWrapper( int signo, Sigfunc *func )
// 'signo' specifies the signal and can be any valid signal 
//  except SIGKILL and SIGSTOP.

{
	struct sigaction	act, oact;

	act.sa_handler = func;
	sigemptyset( &act.sa_mask );
	act.sa_flags   = 0;

	if( sigaction( signo, &act, &oact ) < 0 )
		return( SIG_ERR );

	return( oact.sa_handler );
}

//------------------------------------------------------------
void sigHandler_A( int sig ) 
{
    fflush( stdout ) ;
    printf("\n\n### I (%d) have been nicely asked to PAUSE. by Signal #%3d.\n\n"
           , getpid() , sig );

    pid_t pid = fork();

    if(pid == 0){
        execl("./divider_v4", "divider_v4", NULL);
        perror("execl failed"); 
        exit(1);
    }
    //kill(getpid(), SIGSTOP);

}

//------------------------------------------------------------
void sigHandler_CONT( int sig ) 
{
    fflush( stdout ) ;
    printf("\n\n### I (%d) have been asked to RESUME by Signal #%3d.\n\n"
           , getpid() , sig );

    FOREVER = false;
}

//------------------------------------------------------------

int main(int argc, char* argv[])
{
    if(argc != 3){
        fprintf(stderr, "Usage: %s num1 num2\n", argv[0]);
        exit(1);
    }
    
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);

    pid_t  mypid ;

    mypid = getpid() ;
    printf("\nHELLO! I AM THE COUNT PROCESS WITH ID= %d\n" , mypid );
    
    key_t key = ftok("shmSegment.h", 65);
    
    shmid = shmget(key, SHMEM_SIZE, IPC_CREAT | 0666);
   
    shm_ptr = (shmData*) shmat(shmid, NULL, 0);
   
    shm_ptr->num1 = num1;
    shm_ptr->num2 = num2;

    sigactionWrapper(SIGCONT, sigHandler_CONT);
    sigactionWrapper(SIGTSTP, sigHandler_A);
    
    unsigned i=0;
    while(  FOREVER ){
        printf("%10X\r" , i++ ) ;
        usleep(10000);
        fflush(stdout);
    }
    
    if(shm_ptr->num2 != 0){
        printf("\nCOUNTER: Stopped Counting. The FOREVER flag must have become FALSE\n\n\n");
        printf("COUNTER: Found this ratio in the Shared Memory:     %.3f\n", shm_ptr->ratio);
    }
    if(shmdt(shm_ptr) == -1){
        perror("shmdt failed");
        exit(1);
    }

    if(shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("shmctl failed");
        exit(1);
    }

    printf("\nCOUNTER: Goodbye\n\n");
    
	return 0 ;
}

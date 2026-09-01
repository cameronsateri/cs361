//---------------------------------------------
// Assignment  : Lab-04_a IPC_pipes
// Date        : 9/26/2024
// 
// Authors     : Team 19
//               Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : go.c
//---------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

/*-------------------------------------------------------*/
/* Wrapper function for fork()                           */

pid_t Fork(void)
{
    pid_t n;
    n = fork() ;
    if ( n < 0 ) 
    {
		perror("Fork failed");
		exit(-1) ;
    }
    return n ;
}

#define  READ_END	0
#define  WRITE_END	1
#define  STDIN_FD	0
#define  STDOUT_FD	1

int main(void){
    pid_t pidUser, pidCalc;
    int requestPipe[2];
    int replyPipe[2];

    if(pipe(requestPipe) == -1 || pipe(replyPipe) == -1){
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    printf("Go started and created these pipes\n");
    printf("\tUser-to-Calculator Request Pipe: read=%d write=%d\n", requestPipe[READ_END], requestPipe[WRITE_END]);
    printf("\tCalculator-to-User Request Pipe: read=%d write=%d\n\n", replyPipe[READ_END], replyPipe[WRITE_END]);

    pidUser = Fork();
    if(pidUser == 0)
    {

        close (requestPipe[READ_END]);
        close (replyPipe[WRITE_END]);

        char readFD[10], writeFD[10];
        snprintf(readFD, sizeof(readFD), "%d", requestPipe[WRITE_END]);
        snprintf(writeFD, sizeof(writeFD), "%d", replyPipe[READ_END]);

        if (execlp("./user", "user", readFD, writeFD, NULL) < 0){
            perror("execpl User child Failed");
            exit(-1);
        }
        
    }else{
        pidCalc = Fork();
        if(pidCalc == 0){

            close( requestPipe[WRITE_END]);
            close(replyPipe[READ_END]);
            
            char readFD[10], writeFD[10];
            snprintf(readFD, sizeof(readFD), "%d", requestPipe[READ_END]);
            snprintf(writeFD, sizeof(writeFD), "%d", replyPipe[WRITE_END]);

            if (execlp("./calculator", "calculator", readFD, writeFD, NULL) < 0){
                perror("execlp Calculation child Failed");
                exit(-1);
            }
            
        }else if(pidCalc > 0){
            printf("Go is now waiting for User to terminate\n");
            waitpid(pidUser, NULL, 0);

            printf("Go is now waiting for Calculator to terminate\n\n");
            waitpid(pidCalc, NULL, 0);

        }
            close(requestPipe[READ_END]);
            close(requestPipe[WRITE_END]);
            close(replyPipe[WRITE_END]);
            close(replyPipe[READ_END]);
            printf("The Go process has terminated\n");
    }


}
//---------------------------------------------
// Assignment  : Lab-04_a IPC_pipes
// Date        : 9/26/2024
// 
// Authors     : Team 19
//               Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : user.c
//---------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){
     if( argc != 3){
        fprintf(stderr, "Usage: %s <req_pipe_read> <rply_pipe_write>\n", argv[0]);
        exit(-1);
    }

    int num1, num2;
    char operator;
    //char input[60];

    printf("This is the User process (id = %d).\n", getpid());
    printf("\tUser: Please, enter:    value1    operation    value1\n");

    scanf("%d %c %d", &num1, &operator, &num2);
    
    int request_fd = atoi(argv[1]);
    int reply_fd = atoi(argv[2]);

    printf("\tUser: Sending the following: %d %c %d on FD %d\n", num1, operator, num2, request_fd);

    if(write(request_fd, &num1, sizeof(num1)) < 0){
        perror("Error writing num1");
        exit(-1);
    } 
    if(write(request_fd, &operator, sizeof(operator)) <0 ){
        perror("Error writing num2");
        exit(-1);
    }
    if(write(request_fd, &num2, sizeof(num2)) < 0){
        perror("Error writing num1");
        exit(-1);
    }

    int result;

    if(read(reply_fd, &result, sizeof(result)) < 0){
        perror("Error reading result");
        exit(-1);
    }

    printf("User Received the following result: %d from FD %d\n\n", result, reply_fd);

    close(request_fd);
    close(reply_fd);

    return 0;

}
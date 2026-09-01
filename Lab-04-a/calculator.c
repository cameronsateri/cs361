//---------------------------------------------
// Assignment  : Lab-04_a IPC_pipes
// Date        : 9/26/2024
// 
// Authors     : Team 19
//               Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : calculator.c
//---------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//Helper calcuation funciton.
int calculation(int num1, char operator, int num2){
    switch(operator){
        case '+':
            return num1 + num2;
            break;
        case '-':
            return num1 - num2;
            break;
        case '*':
            return num1 * num2;
            break;
        case '/':
            if(num2 == 0){
                fprintf(stderr, "Error: Division by zero.\n");
                exit(1);
            }
            return num1 / num2;
            break;
        default:
            fprintf(stderr, "Unknown operation \n");
            exit(1);
    }
}

int main(int argc, char *argv[]){

    int request_fd = atoi(argv[1]);
    int reply_fd = atoi(argv[2]);

    printf("This is the Calculator process (id = %d).\n", getpid());;
    printf("Calculator Waiting to receive from FD %d\n", request_fd);

    int num1, num2;
    char operator;

    if(read(request_fd, &num1, sizeof(num1)) <= 0){
        perror("Error reading num1");
        exit(1);
    }
    if(read(request_fd, &operator, sizeof(operator)) <= 0){
        perror("Error reading num2");
        exit(1);
    }
    if(read(request_fd, &num2, sizeof(num2)) <= 0){
        perror("Error reading num3");
        exit(1);
    }

    printf("\tCalculator received the following: %d %c %d from FD %d\n", num1, operator, num2, request_fd);

    int result = calculation(num1, operator, num2);
    
    if(write(reply_fd, &result, sizeof(result)) <= 0){
        perror("Error writing result");
        exit(1);
    }

    printf("Calculator sent the following to User: %d on FD %d\n", result, reply_fd);
    close(request_fd);
    close(reply_fd);

    return 0;
}

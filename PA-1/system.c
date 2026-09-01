//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/19/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : system.c
//---------------------------------------------

#include "system.h"
#include <stdio.h>

// Global variable for tracking number of payment attempts
int payAttempts = 0 ;
int order_size = 0 ; // Number of items to manufacture
//---------------------------------------------------------------------
// Functions for interacting with the outside world
void chargeClient()
{
    printf("Client has been charged\n");
}
//---------------------------------------------------------------------
void paymentRejected()
{
    printf("Payment Rejected\n");
}
//---------------------------------------------------------------------
void refund()
{
    printf("Refund Issued\n");
}
//---------------------------------------------------------------------
void startWarranty()
{
    printf("Warranty has started\n");
}
//---------------------------------------------------------------------
// Functions for controlling global behavior
void getOrderSize()
{
    printf("Got an order of size = %d items\n" , order_size);
}
void resetAttempts()
{
    payAttempts = 0;
    printf("Invalid Payment-Attempts Reset to %d\n" , payAttempts );
}
//---------------------------------------------------------------------
void incrementAttempts()
{
    payAttempts ++;
    printf("Invalid Payment-Attempts Incremented to %d\n" , payAttempts);
}
//---------------------------------------------------------------------
void updateStats( statsSetting value)
{
    printf("Updating Statistics for ");
    switch(value)
    {
        case DONE: 
            printf("SUCCESSFUL orders\n");
            break ;
        case LOST: 
            printf("LOST packages\n");
            break ;
        case FAIL: 
            printf("FAILED manufacturing\n") ;
            break ;
    }
}

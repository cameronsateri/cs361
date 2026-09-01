//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : system.h
//---------------------------------------------

#ifndef system_h
#define system_h
#include <stdio.h>

// Event settings
typedef enum
  {
    ORDER_RECEIVED,
    INVALID_PAYMENT,
    VALID_PAYMENT,
    MANUFACTURE_COMPLETED,
    MANUFACTURE_FAIL,
    SHIPMENT_ARRIVE,
    SHIPMENT_LOST
  } event;
#define INVALID_EVENT -1

// Update settings
typedef enum
  {
    DONE,
    LOST,
    FAIL
  } statsSetting;

// Create functions in system.c
void chargeClient();

void paymentRejected();

void refund();

void startWarranty();

void getOrderSize();

void resetAttempts();

void incrementAttempts();

void updateStats( statsSetting value);

#endif
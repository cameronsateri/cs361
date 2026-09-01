//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : processing.h
//---------------------------------------------


#ifndef processing_h
#define processing_h

#include "state.h"


// Declare all of the functions performed
// when in the accepting state.
static state_t* invalid_payment();
static state_t* valid_payment();
static void entry_to_processing();
static void getPymntMethod();
extern void incrementAttempts();
extern void paymentRejected();
extern int payAttempts;

#endif
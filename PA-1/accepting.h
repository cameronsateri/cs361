//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : accepting.h
//---------------------------------------------

#ifndef accepting_h
#define accepting_h

#include "state.h"

// Declare all of the functions performed
// when in the accepting state.
static state_t* order_received();
extern void getOrderSize();
extern void resetAttempts();

#endif
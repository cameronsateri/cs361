//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : statemodel.h
//---------------------------------------------

#ifndef statemodel_h
#define statemodel_h

#include <stdlib.h>
#include "system.h"
#include "state.h"

// define states
extern state_t accepting;
extern state_t processing;
extern state_t manufacturing;
extern state_t shipping;

// declare functions
void printStateName(void) ;
void handle_event( event current_event );

#endif



//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : manufacturing.h
//---------------------------------------------

#ifndef manufacturing_h
#define manufacturing_h

#include "state.h"
#include "statemodel.h"


// Declare all of the functions performed
// when in the accepting state.
static state_t* manufacture_failed();
static state_t* manufacture_completed();
static void entry_to_manufacturing();
static void exit_from_manufacturing();
extern void updateStats(statsSetting value);
extern void chargeClient();
static void dispatchFactoryLines();
static void shutDownFactoryLines();

#endif
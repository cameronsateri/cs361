//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : shipping.h
//---------------------------------------------


#ifndef shipping_h
#define manufacturing_h

#include "state.h"
#include "statemodel.h"

// Declare all of the functions performed
// when in the accepting state.
static state_t* shipment_arrived();
static state_t* shipment_lost();
static void entry_to_shipping();
static void getAddress();
extern void startWarranty();
extern void updateStats(statsSetting value);
extern void refund();

#endif
//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : manufacturing.c
//---------------------------------------------

#include "manufacturing.h"
#include "system.h"
#include "statemodel.h"  

extern state_t* default_event_handler(); // For events that don't effect the state.
extern void default_action(); // For entry and exits.

// Create the object of the manufacturing state.
state_t manufacturing = {
    default_event_handler,        // order_recieved
    default_event_handler,        // invalid_payment
    default_event_handler,        // valid_payment
    manufacture_completed,        // manufacture_completed
    manufacture_failed,           // manufacture_failed
    default_event_handler,        // shipment_arrived
    default_event_handler,        // shipment_failed
    entry_to_manufacturing,       // entry_to
    exit_from_manufacturing       // exit_from
};

// Adjusting state for a completed manufacture.
state_t* manufacture_completed(){
    exit_from_manufacturing();
    chargeClient();
    return &shipping;
}

// Adjusting state for a failed manufacture.
state_t* manufacture_failed(){
    exit_from_manufacturing();
    updateStats(FAIL);
    return &accepting;
}

void dispatchFactoryLines(){
    printf("Dispatching Factory Lines\n");
}

void shutDownFactoryLines(){
    printf("Shutting Down Factory Lines\n");
}

//Entry action to manufacturing.
void entry_to_manufacturing(){
    dispatchFactoryLines();
}

//Exit action for manufacturing.
void exit_from_manufacturing(){
    shutDownFactoryLines();
}

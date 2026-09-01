//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : accepting.c
//---------------------------------------------

#include "accepting.h"
#include "system.h"
#include "statemodel.h" 
#include <stdio.h>

extern state_t* default_event_handler(); // For events that don't effect the state.
extern void default_action();  // For entry and exits.

// Create the object of the accepting state.
state_t accepting = {
    order_received,             // order_recieved
    default_event_handler,      // invalid_payment
    default_event_handler,      // valid_payment
    default_event_handler,      // manufacture_complete
    default_event_handler,      // manufacture_failed
    default_event_handler,      // shipment_arrived
    default_event_handler,      // shippment_failed
    default_action,             // entry_to
    default_action              // exit_from
};

// Handling the order recieved and adjusting the state.
state_t* order_received(){
    getOrderSize();
    resetAttempts();
    return &processing;
};

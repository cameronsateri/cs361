//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : shipping.c
//---------------------------------------------

#include "shipping.h"
#include "system.h"
#include "statemodel.h"

extern state_t* default_event_handler(); // For events that don't effect the state.
extern void default_action(); // For entry and exits.

// Creat the object of the shipping state.
state_t shipping = {
    default_event_handler,        //order_recieved
    default_event_handler,        //invalid_payment
    default_event_handler,        //valid_payment
    default_event_handler,        //manufacture_completed
    default_event_handler,        //manufacture_failed
    shipment_arrived,             //shipment_arrived
    shipment_lost,                //shipment_lost
    entry_to_shipping,            //entry_to
    default_action                //exit_from
};

// Adjusting the state when the shipment arrived.
state_t* shipment_arrived(){
    startWarranty();
    updateStats(DONE);
    return &accepting;
}

// Adjusting the state when the shipment got lost.
state_t*shipment_lost(){
    refund();
    updateStats(LOST);
    return &accepting;
}

// Entry action to shipping state.
void entry_to_shipping(){
    getAddress();
}

void getAddress(){
    printf("Getting Ship-To Address\n");
}


//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : processing.c
//---------------------------------------------


#include "processing.h"
#include "system.h"
#include "statemodel.h" 

extern state_t* default_event_handler(); // For events that don't effect the state.
extern void default_action(); // For entry and exits.

// Create the object of the processing state.
state_t processing = {
    default_event_handler,          //order_recieved
    invalid_payment,                //invalid_payment
    valid_payment,                  //valid_payment
    default_event_handler,          //manufacture_completed
    default_event_handler,          //manufacture_failed
    default_event_handler,          //shipment_arrived
    default_event_handler,          //shippment_failed
    entry_to_processing,            //entry_to
    default_action                  //exit_from

};

// Adjusting state for invalid payment based on 
// if the gaurd (number of paymentAttempts) is met.
state_t* invalid_payment(){
    incrementAttempts();
    if(payAttempts == 3){
        paymentRejected();
        return &accepting;
    }
    return &processing;
}

// Adjusting state for a valid payment.
state_t* valid_payment(){
    return &manufacturing;
}

void getPymntMethod(){
    printf("Getting Payment Method\n");
}

// Entry actions to processing state.
void entry_to_processing(){
    getPymntMethod();
}
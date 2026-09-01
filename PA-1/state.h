//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : state.h
//---------------------------------------------

#ifndef state_h
#define state_h


// Create type definitions
typedef struct state state_t;

typedef state_t * event_handler(void);

typedef void action(void);

// Define state format
struct state
{
    event_handler* order_received;
    event_handler* invalid_payment;
    event_handler* valid_payment;
    event_handler* manufacture_completed;
    event_handler* manufacture_failed;
    event_handler* shipment_arrived;
    event_handler* shipment_lost;
    action* entry_to;
    action* exit_from;
};

#endif
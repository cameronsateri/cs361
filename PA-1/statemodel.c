//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : statemodel.c
//---------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "statemodel.h"

extern state_t* current_state ;

void handle_event( event current_event )
{
    // Adjusting state mased on current event.
    state_t* next_state = NULL;
    switch (current_event)
    {
        case ORDER_RECEIVED:
            next_state = current_state->order_received();
            break;
        case INVALID_PAYMENT:
            next_state = current_state->invalid_payment();
            break;
        case VALID_PAYMENT:
            next_state = current_state->valid_payment();
            break;
        case MANUFACTURE_COMPLETED:
            next_state = current_state->manufacture_completed();
            break;
        case MANUFACTURE_FAIL:
            next_state = current_state->manufacture_failed();
            break;
        case SHIPMENT_ARRIVE:
            next_state = current_state->shipment_arrived();
            break;
        case SHIPMENT_LOST:
            next_state = current_state->shipment_lost();
            break;
    }

    if (next_state != NULL)
    {
        current_state = next_state;  // change state
        printStateName();
        current_state->entry_to();  // enter next state
    }
}

// Formatted print output.
void printStateName(void)
{
    printf("\n*-*-*-*-*-*-*-*-*-*-*-*\nState: ");
    if ( current_state == &accepting )
        printf( "ACCEPTING" );
    else if ( current_state == &processing )
        printf( "PROCESSING" );
    else if ( current_state == &manufacturing )
            printf( "MANUFACTURING" );
    else if ( current_state == &shipping )
            printf( "SHIPPING" );
    printf("\n*-*-*-*-*-*-*-*-*-*-*-*\n");
}


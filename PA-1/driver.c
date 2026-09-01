//---------------------------------------------
// Assignment  : PA-1 FSM
// Date        : 9/20/2024
// 
// Authors     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//               Cameron Sateri satericj@dukes.jmu.edu
//
// File Name   : driver.c
//---------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "statemodel.h"

// Define the initial state.
state_t* current_state = &accepting;  // Starting state.

int main(void)
{
    event current_event;
    int key;

    printStateName();
    current_state->entry_to();

    // Continue while input key is valid.
    while ((key=getchar()) != 'X')
    {
        if (key == '\n') continue;

        current_event = INVALID_EVENT;
        // Adjust event based on input key.
        switch (key)
        {
            case 'O':
                current_event = ORDER_RECEIVED;
                puts("Event: ORDER_RECEIVED");
                break;
            case 'V':
                current_event = VALID_PAYMENT;
                puts("Event: VALID_PAYMENT");
                break;
            case 'I':
                current_event = INVALID_PAYMENT;
                puts("Event: INVALID_PAYMENT");
                break;
            case 'F':
                current_event = MANUFACTURE_FAIL;
                puts("Event: MANUFACTURE_FAILED");
                break;
            case 'C':
                current_event = MANUFACTURE_COMPLETED;
                puts("Event: MANUFACTURE_COMPLETED");
                break;
            case 'R':
                current_event = SHIPMENT_ARRIVE;
                puts("Event: SHIPMENT_ARRIVED");
                break;
            case 'L':
                current_event = SHIPMENT_LOST;
                puts("Event: SHIPMENT_LOST");
                break;
            default:
                current_event = INVALID_EVENT;
                puts("Event: INVALID_EVENT");
        }

        // Handle valid events
        if (current_event != INVALID_EVENT){
            handle_event(current_event);
        }
    }
    printf("\n");
    fflush(stdout);
}
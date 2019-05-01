/*
 * File:   clock.c
 * Author: apatc
 *
 * Created on April 14, 2019, 4:04 PM
 */


#include <xc.h>

#include "clock.h"

time_t ClockCurrentTime = 0;


static void CLOCK_timerCallback();

void CLOCK_init()
{
    
    // redirect MCC's timer callback to our custom function
    //TMR0_SetInterruptHandler(CLOCK_timerCallback);
    
    // if not using MCC, init Timer0 here
    T0CON1bits.T0CS = 0b010; 
    T0CON1bits.CKPS = 0b0110; //0x0010

    TMR0H = 249;
        
    PIE0bits.TMR0IE = 1; 
    //INTCONbits.GIE = 1; 
    
    T0CON0bits.EN = 1;
}

time_t CLOCK_getTime() {
    return ClockCurrentTime;
}

static void CLOCK_timerCallback() {
    //currentTime++;
}
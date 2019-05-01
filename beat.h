#ifndef BEAT_H
#define	BEAT_H



//static void adcThresholdHandler();


#include <stdbool.h>
#include "mcc_generated_files/adcc.h"
#include "clock.h"
#include "tm1650.h"
#include "stdio.h"


void BEAT_task();
void BEAT_init();
bool BEAT_detected();
void putch(char character);





#endif
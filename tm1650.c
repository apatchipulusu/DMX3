/*
 * File:   tm1650.c
 * Author: apatc
 *
 * Created on March 26, 2019, 4:14 PM
 */

#include "tm1650.h"



void TM1650_init(){
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    //SYSTEM_Initialize();
    TM1650_enable(true);
}

static void writeData(uint8_t address, uint8_t data){
    I2C1_MESSAGE_STATUS status = I2C1_MESSAGE_PENDING;
    uint8_t dataToSend = data;
    I2C1_MasterWrite(&dataToSend, 1, address, &status);
    while( status == I2C1_MESSAGE_PENDING );
}

void TM1650_setDigit(uint8_t digit, char num, bool dp){
    const uint8_t charTable[] = 
    {
    0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00,
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53,
    0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F,
    0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, 
    0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C,
    0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00 
    };
    uint8_t data = charTable[num-32];
    if(dp == true){
        data |= 0b10000000;
    }
    switch (digit)
    {
            case 0:
                writeData(0x34, data);
                break;
            case 1:
                writeData(0x35, data);
                break;
            case 2:
                writeData(0x36, data);
                break;
            case 3:
                writeData(0x37, data);
                break;
            default: 
                break;
    }
}

void putch(char dataToYeet){
    if(dataToYeet == 13){
        writeData(0x34, 0x00);
        writeData(0x35, 0x00);
        writeData(0x36, 0x00);
        writeData(0x37, 0x00);
        count = 0;
    }else if(dataToYeet != 13){
        TM1650_setDigit(count, dataToYeet, false);
        count++;
    }
}

void TM1650_fastPrintNum(uint16_t num){
    char array[4];
    if(num < 10){
        array[0] = (num % 10) + 48;//
        TM1650_setDigit(0,' ', 0);
        TM1650_setDigit(1,' ', 0);
        TM1650_setDigit(2,' ', 0);
        TM1650_setDigit(3,array[0], 0);
    }else if(num < 100 && num >= 10){
        array[0] = (num % 10) + 48;//
        array[1] = (((num - (num % 10))/10) % 10) + 48;
        TM1650_setDigit(0,' ', 0);
        TM1650_setDigit(1,' ', 0);
        TM1650_setDigit(2,array[1], 0);
        TM1650_setDigit(3,array[0], 0);
    }else if(num < 1000 && num >= 100){
        array[0] = (num % 10) + 48;
        array[1] = ((num - (num % 10))/10) % 10 + 48;
        array[2] = ((num - (((num - (num % 10))/10) % 10))/100) %10 + 48;
        TM1650_setDigit(0,' ', 0);
        TM1650_setDigit(1, array[2], 0);
        TM1650_setDigit(2, array[1], 0);
        TM1650_setDigit(3, array[0], 0);
    }else if(num < 10000 && num >= 1000){
        array[0] = (num % 10) + 48;
        array[1] = ((num - (num % 10))/10) % 10 + 48;
        array[2] = ((num - (((num - (num % 10))/10) % 10))/100) %10 + 48;
        array[3] = ((num - (((num - (((num - (num % 10))/10) % 10))/100) %10))/1000) % 10 + 48;
        TM1650_setDigit(0, array[3], 0);
        TM1650_setDigit(1, array[2], 0);
        TM1650_setDigit(2, array[1], 0);
        TM1650_setDigit(3, array[0], 0);
    }else{
        TM1650_setDigit(0, 'E', 0);
        TM1650_setDigit(1, 'E', 0);
        TM1650_setDigit(2, 'E', 0);
        TM1650_setDigit(3, 'E', 0);
    }
}

void TM1650_enable(bool enable){
    if(enable == true){
        writeData(0x24, 0x01);
        displayState = true;
    }else if(enable == false){
        writeData(0x24, 0x00);
        displayState = false;
    }
}

bool TM1650_isEnabled(){
    return displayState;
}
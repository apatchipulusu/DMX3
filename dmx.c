/*
 * File:   DMX_Source.c
 * Author: apatc
 *
 * Created on April 7, 2019, 3:38 PM
 */


#include "dmx.h"
#include "mcc_generated_files/eusart1.h"
#include "controller.h"
#include "beat.h"

uint8_t input[513]; 

int point;

static bool dmx_mode = true;
static time_t lastTime;
time_t dmx_lastActiveTime;
int R = 255;
int G = 0;
int B = 0;
int W = 128;
int ani1_state = 0;
int animationSpeed = 1;

void handler() {
    LATC5 = 1;
    if(RC1STAbits.OERR == 1){
                RC1STAbits.CREN = 0;
                RC1STAbits.CREN = 1;
            }
            uint8_t dataIn = RC1REG;
            if(RC1STAbits.FERR == 1){
                point = 0;
            }else if(point > 513){
                //OOF
            }else{
                input[point] = dataIn;
                point++;
            }
    LATC5 = 0;
}

void DMX_init() {
    EUSART1_SetRxInterruptHandler(handler);
    TRISB4 = 1;
    ANSELB = 0x00;
    //*point = 0;
    point = 0;
}

void UART_init(){
    SP1BRG = 0x03;//0x03
    
    TX1STAbits.BRGH = 1;
    BAUD1CONbits.BRG16 = 1;
    
    //RB4PPS = 0x0F;
    RX1PPS = 0b00001100;
    
    TX1STAbits.SYNC = 0;
    
    RC1STAbits.SPEN = 1;
    
    RC1STAbits.RX9 = 0;//8 bit reception 
    RC1STAbits.CREN = 1;//Receive enable
    TX1STAbits.TXEN = 0;//transmit disabled
    
    TRISB4 = 1;
    ANSELB = 0x00;
    //*point = 0;
    point = 0;
    PIE3bits.RC1IE = 1;
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
}

void LED_init(){
    CCP1CONbits.MODE = 0b1100;
    CCP2CONbits.MODE = 0b1100;
    CCP3CONbits.MODE = 0b1100;
    CCP4CONbits.MODE = 0b1100;
    //T2CONbits.CKPS = 0b101;//101 for 32 prescaler
    CCP1CONbits.CCP1EN = 1;
    CCP2CONbits.CCP2EN = 1;
    CCP3CONbits.CCP3EN = 1;
    CCP4CONbits.CCP4EN = 1;
    T2CLKCON = 0x01;
    //
    TRISC7 = 0; //B7
    TRISC6 = 0; //C7
    TRISC3 = 0; //C6
    TRISC4 = 0; //C3
    //Change to connect to dmx
    RC7PPS = 0x09;//RED //RB7
    RC6PPS = 0x0A;//GREEN //RC7
    RC3PPS = 0x0B;//BLUE //RC6
    RC4PPS = 0x0C;//WHITE //RC3
    // 
    CCPR1H = 0x00;
    CCPR1L = 0x04;
    CCPR2H = 0x00;
    CCPR2L = 0x04;
    CCPR3H = 0x00;
    CCPR3L = 0x04;
    CCPR4H = 0x00;
    CCPR4L = 0x04;
    T2CONbits.ON = 1;
}

void LED_setColor(uint8_t R, uint8_t G, uint8_t B, uint8_t W){
    if(R < 256 && G < 256 && B < 256 && W < 256){
    R = ((float)R/2048)*(4000000/(122 * 32));
    G = ((float)G/2048)*(4000000/(122 * 32));
    B = ((float)B/2048)*(4000000/(122 * 32));
    W = ((float)W/2048)*(4000000/(122 * 32));
    CCPR1H = R >> 8;
    CCPR1L = R;
    CCPR2H = G >> 8;
    CCPR2L = G;
    CCPR3H = B >> 8;
    CCPR3L = B;
    CCPR4H = W >> 8;
    CCPR4L = W;
    }
}

void LED_setBrightness(unsigned int duty){
    duty = ((float)duty/1024)*(4000000/(122 * 32));
    CCPR1H = duty >> 8;
    CCPR1L = duty;
}

void DMX_Task(){
//    time_t time = CLOCK_getTime();
//
//    if (time - lastTime < DMX_BUTTON_SCAN_INTERVAL)//check every 100ms
//        return;
//
//    lastTime = time;
    
    if(CLOCK_getTime() - dmx_lastActiveTime >= 50){
        dmx_mode = false;
    }else{
        dmx_mode = true;
    }
    
    if(DMX_Mode()){
        LED_setColor(input[address],input[address+1],input[address+2],input[address+3]);
    }else{
        if(address == 1){
            time_t time = CLOCK_getTime();
            // only run every 10 ms
            if (time - lastTime > 100)
                return;
            lastTime = time;
            if(color == 0 && BEAT_detected()){
                LED_setColor(127,0,0,0);
                color = 1;
            }
            else if(color == 1 && BEAT_detected()){
                LED_setColor(0,127,0,0);
                color = 2;
            }
            else if(color == 2 && BEAT_detected()){
                LED_setColor(0,0,127,0);
                color = 3;
            }
            else if(color == 3 && BEAT_detected()){
                LED_setColor(0,0,0,127);
                color = 0;
            }
        }else if(address == 2){
            LED_setColor(127,127,127,127);
        }else if(address == 3){
            LED_setColor(127,0,0,0);
        }else if(address == 4){
            LED_setColor(0,127,0,0);
        }else if(address == 5){
            LED_setColor(0,0,127,0);
        }else if(address == 6){
            LED_setColor(0,0,0,127);
        }else if(address == 7){
            animation1();
        }
        
    }
}

bool DMX_Mode(){
    if (dmx_mode) {
        //detected = false;
        return true;
    }
    return false;
}

void animation1(){
    if(ani1_state == 0){//R>=0 && B<128 && 
        R-=animationSpeed; B+=animationSpeed;
        LED_setColor(R, G, B, W);
        if(R <= 0){
            ani1_state = 1;
        }
    }else if(ani1_state == 1){//B>=0 && G<128 && 
        B-=animationSpeed; G+=animationSpeed;
        LED_setColor(R, G, B, W);
        if(B <= 0){
            ani1_state = 2;
        }
    }else if(ani1_state == 2){//G>=0 && R<128 && 
        G-=animationSpeed; R+=animationSpeed;
        LED_setColor(R, G, B, W);
        if(G <= 0){
            ani1_state = 0;
        }
    }
}

//void ADC_init(){
//    ADCON0bits.FM = 1;
//    ADCON0bits.CS = 1;
//    ADPCH = 0b001110;
//    TRISB6 = 1;
//    ANSELB = 0x40;
//    ADCON0bits.ON = 1;
//}
//
//uint16_t ADC_getValue(){
//    uint16_t finalResult = 0x0000;
//    ADCON0bits.GO = 1;
//    while(ADCON0bits.GO);
//    finalResult = ADRESH<<8;
//    finalResult |= ADRESL;
//    return finalResult/4;//To output something within proper range
//}
//
//
//
//void CLOCK_init()
//{
//    // redirect MCC's timer callback to our custom function
//    //TMR0_SetInterruptHandler(CLOCK_timerCallback);
//    
//    // if not using MCC, init Timer0 here
//    T0CON1bits.T0CS = 0b010; 
//    T0CON1bits.CKPS = 0b0010; 
//
//    TMR0H = 249;
//        
//    PIE0bits.TMR0IE = 1; 
//    //INTCONbits.GIE = 1; 
//    
//    T0CON0bits.EN = 1;
//}
//
//time_t CLOCK_getTime() {
//    return currentTime;
//}
//
//void CLOCK_timerCallback() {
//    currentTime++;
//}


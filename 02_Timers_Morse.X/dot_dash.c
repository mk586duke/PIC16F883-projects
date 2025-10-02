///*
// * File:   main.c
// * Author: thomaskim
// *
// * Created on September 9, 2025, 10:04 AM
// */
//
//// CONFIG1
//#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
//#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
//#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
//#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
//#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
//#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
//#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
//#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
//#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
//#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
//
//// CONFIG2
//#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
//#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <xc.h>
//#include <htc.h>
//#include <pic.h>
//#define _XTAL_FREQ 4000000
//
//#define ONE     0b00010100
//#define TWO     0b10110011
//#define THREE   0b10110110
//#define FOUR    0b11010100
//#define FIVE    0b11100110
//#define SIX     0b11100111
//#define SEVEN   0b00110100
//#define EIGHT   0b11110111
//#define NINE    0b11110110
//#define ZERO    0b01110111
//#define A       0b11110101
//#define B       0b11000111
//#define C       0b01100011
//#define D       0b10010111
//#define E       0b11100011
//#define F       0b11100001
//#define DECIMAL 0b00001000
//#define DASH    0b10000000
//
//unsigned char LOW = 0xFF;
//unsigned char HIGH = 0xFF;
//unsigned char options[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, A, B, C, D, E, F};
//        
//
//// #pragma config statements should precede project file includes.
//// Use project enums instead of #define for ON and OFF.
//
////static unsigned int MAX_CLOCK = 1 << 19;
//static unsigned int start_400ms = 65536 - 50000;//(1 << 15) - 400000/8;
//
//
//static inline void tmr1_load_400ms(void) {
//    TMR1 = start_400ms;  // TODO: maybe just add this
//}
//
//static inline unsigned int get_mics() {
//    return (TMR1 - start_400ms);
//}
//
//static void tmr1_init_400ms_tick(void) {
//    // Timer1: Fosc/4, prescale 1:8
//    T1CONbits.TMR1ON  = 0;    // stop while configuring
//    T1CONbits.T1CKPS0 = 1;    // 1:8
//    T1CONbits.T1CKPS1 = 1;
//    T1CONbits.TMR1CS  = 0;    // Fosc/4
//    T1CONbits.T1OSCEN = 0;    // no T1 oscillator
//    TMR1 = 0;
//    tmr1_load_400ms();
//    PIR1bits.TMR1IF = 0;      // clear flag
//    PIE1bits.TMR1IE = 1;      // enable TMR1 interrupt
//    INTCONbits.PEIE  = 1;     // enable peripheral interrupts
//    T1CONbits.TMR1ON = 1;     // start
//}
//
//int overflow400 = 0;
//int displaying = 0;
//void __interrupt() isr(void) 
//{
//    // ---- Timer1: millisecond tick ----
//    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
//        tmr1_load_400ms();  // Reset the timer
//        PIR1bits.TMR1IF = 0;
//        if (overflow400 > 5 && displaying==1) {
//            PORTC = 0;
//            displaying = 0;
//            overflow400 = 0;  // time-out not started
//        } else if (displaying == 0 && overflow400==3) {
//            PORTC = E;
//            displaying = 1;
//            overflow400 = 1;
//        } else if (overflow400 > 0) {
//            overflow400++;
//        }
//    }
//    if (INTCONbits.RBIE && INTCONbits.RBIF && displaying==0) {
//        if (PORTBbits.RB0 == 1) {  // Button Press (start timer)
//            TMR1 = 0;
//            tmr1_load_400ms();  // FIXME: too long hold & error will break
//            overflow400 = 1;  // time-out started
//        } else if (PORTBbits.RB0 == 0 && overflow400 > 0) { // Button Release (capture timer)
//            unsigned int mics = get_mics();
//            if (mics < 30000/8) {
//                PORTC = E;
//            } else if (mics < 25000){//200000/8) {
//                PORTC = DECIMAL;
//            } else {
//                PORTC = DASH;
//            }
//            overflow400 = 2;
//            displaying = 1;
//        }
//        INTCONbits.RBIF = 0;   // clear IOC flag
//    }
//    
//}
//
//
//void main(void) 
//{   
//    ANSEL = 0X00; // port a is digital i/o
//    ANSELH = 0X00; // port b is digital i/o
//    
//    TRISA = 0X00; // port a is output
//    TRISB = 3; // port b is input
//    TRISC = 0X00; // port c is output
//    
//
//    INTCONbits.GIE = 1; // enable global interrupts
//    INTCONbits.RBIE = 1; // enable change-on-port-b interrupt
//    IOCB = 0X03; // enable interrupt on change for RB0
//    
//    tmr1_init_400ms_tick();
//    
//    PORTC = 0;
//    PORTAbits.RA0 = 0;
//    PORTAbits.RA1 = 1;  
//    
//    while(1) {
//    }
//    
//}
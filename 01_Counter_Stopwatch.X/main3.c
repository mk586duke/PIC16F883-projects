/*
 * File:   main.c
 * Author: thomaskim
 *
 * Created on September 9, 2025, 10:04 AM
 */

// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <htc.h>
#include <pic.h>
#define _XTAL_FREQ 4000000

#define ONE     0b00010100
#define TWO     0b10110011
#define THREE   0b10110110
#define FOUR    0b11010100
#define FIVE    0b11100110
#define SIX     0b11100111
#define SEVEN   0b00110100
#define EIGHT   0b11110111
#define NINE    0b11110110
#define ZERO    0b01110111
#define DECIMAL 0b00001000

unsigned char LOW = 0xFF;
unsigned char HIGH = 0xFF;
unsigned char options[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};
        

void display(int number) {
    if (number < 100) {
        int low = number % 10;
        LOW = options[low];
        int high = number / 10;
        HIGH = options[high] | DECIMAL;
        
    } else {
        int norm = number / 10;
        int low = norm % 10;
        LOW = options[low];
        int high = norm / 10;
        HIGH = options[high];
    }
}


// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

volatile unsigned int  ms_bucket = 0;      // 0..99 (for 100 ms)
volatile unsigned long centiseconds = 0;   // milliseconds / 100 since boot


int paused = 0;
int last_button1 = 0;
int last_button2 = 0;

static inline void tmr1_load_1ms(void) {
    TMR1H = 0xFF;
    TMR1L = 0x7D;
}

static void tmr1_init_1ms_tick(void) {
    // Timer1: Fosc/4, prescale 1:8
    T1CONbits.TMR1ON  = 0;    // stop while configuring
    T1CONbits.T1CKPS0 = 1;    // 1:8
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.TMR1CS  = 0;    // Fosc/4
    T1CONbits.T1OSCEN = 0;    // no T1 oscillator
    tmr1_load_1ms();
    PIR1bits.TMR1IF = 0;      // clear flag
    PIE1bits.TMR1IE = 1;      // enable TMR1 interrupt
    INTCONbits.PEIE  = 1;     // enable peripheral interrupts
    T1CONbits.TMR1ON = 1;     // start
}


void __interrupt() isr(void) 
{
    // ---- Timer1: millisecond tick ----
    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
        tmr1_load_1ms();
        PIR1bits.TMR1IF = 0;

        // 1 ms elapsed
        if (!paused && ++ms_bucket >= 100) {          // 100 ms -> 1 centisecond increment
            ms_bucket = 0;
            if (centiseconds < 999) {
                ++centiseconds;                // this is milliseconds / 100
                display(centiseconds);
//                display((int)(centiseconds & 0x7FFFFFFF)); // cast to int for your display()
            }
           
        }
    }

    // ---- Port B change (your buttons) ----
    if (INTCONbits.RBIE && INTCONbits.RBIF) {
        if (PORTBbits.RB0 == 1 && last_button1 == 0) { // start/pause
            paused = !paused;
        } else if (PORTBbits.RB1 == 1 && last_button2 == 0) { // reset
            centiseconds = 0;
            ms_bucket = 0;
        }
        INTCONbits.RBIF = 0;   // clear IOC flag
        last_button1 = PORTBbits.RB0;
        last_button2 = PORTBbits.RB1;
    }
}


void main(void) 
{   
    ANSEL = 0X00; // port a is digital i/o
    ANSELH = 0X00; // port b is digital i/o
    
    TRISA = 0X00; // port a is output
    TRISB = 3; // port b is input
    TRISC = 0X00; // port c is output
    

    INTCONbits.GIE = 1; // enable global interrupts
    INTCONbits.RBIE = 1; // enable change-on-port-b interrupt
    IOCB = 0X03; // enable interrupt on change for RB0
    
    tmr1_init_1ms_tick();
    
    PORTC = 0XFF;
    
    while(1) {
        PORTC = LOW;
        PORTAbits.RA0 = 0;
        PORTAbits.RA1 = 1;
        __delay_ms(1);
        PORTC = HIGH;
        PORTAbits.RA0 = 1;
        PORTAbits.RA1 = 0;
        __delay_ms(1);
       
    }
    
}
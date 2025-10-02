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
        HIGH = options[high];  // | DECIMAL
        
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
int counter = 0;
int last_button1 = 0;
int last_button2 = 0;
void __interrupt() isr(void) 
{
//    __delay_ms(1);
    if(INTCONbits.RBIF == 1)
    {
        if(PORTBbits.RB0 == 1 && last_button1 == 0 && counter<99) // increment
        {
            counter++;
            display(counter);
        }
        else if (PORTBbits.RB1 == 1 && last_button2 == 0 && counter>0)
        {
            counter--;
            display(counter);
        }
        INTCONbits.RBIF = 0; // reset the external interrupt flag
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
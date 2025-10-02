/*
 * File:   main.c
 * Author: minseokim
 *
 * Created on September 9, 2025, 10:04 AM
 */


#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <htc.h>
#include <pic.h>
#define _XTAL_FREQ 4000000

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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


void main(void) 
{   
    ANSEL = 0X00; // port a is digital i/o
    ANSELH = 0X00; // port b is digital i/o
    TRISA = 0X00; // port a is output
    TRISB = 0X01; // port b is input
    
    
    while(1)
    {
        __delay_ms(1000);
        PORTAbits.RA0 = 1;
        __delay_ms(1000);
        PORTAbits.RA0 = 0;
    }
}

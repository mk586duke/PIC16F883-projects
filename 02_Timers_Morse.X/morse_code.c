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
#include <string.h>
#include <stdint.h>
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
#define A       0b11110101
#define B       0b11000111
#define C       0b01100011
#define D       0b10010111
#define E       0b11100011
#define F       0b11100001
#define DECIMAL 0b00001000
#define DASH    0b10000000


unsigned char LOW = 0xFF;
unsigned char HIGH = 0xFF;
unsigned char options[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, A, B, C, D, E, F};
        
int overflow400 = 0;
int displaying = 0;
int pressed = 0;

// Morse Code (0=dot, 1=dash)
typedef struct {
    const char *morse;  // Morse code string (with '.'=dot, '-'=dash)
    int value;          // hex value 0..15
} MorseHex;

static const MorseHex morse_hex_table[] = {
    {"-----", 0x0}, {".----", 0x1}, {"..---", 0x2}, {"...--", 0x3},
    {"....-", 0x4}, {".....", 0x5}, {"-....", 0x6}, {"--...", 0x7},
    {"---..", 0x8}, {"----.", 0x9},
    {".-", 0xA},    // A
    {"-...", 0xB},  // B
    {"-.-.", 0xC},  // C
    {"-..", 0xD},   // D
    {".", 0xE},     // E
    {"..-.", 0xF},  // F
};

/* Streaming buffer */
typedef struct {
    char buf[8];   // Morse code (max length = 5 for digits, +1 margin)
    int len;
} MorseStream;

void ms_init(MorseStream *ms) { ms->len = 0; ms->buf[0] = '\0'; }
void ms_push_bit(MorseStream *ms, int bit) {
    if (ms->len >= 7) return;
    ms->buf[ms->len++] = (bit == 0) ? '.' : '-';
    ms->buf[ms->len] = '\0';  // Null terminate the string
}
void ms_display(MorseStream *ms) {
    int out;
    for (int i = 0; i < sizeof(morse_hex_table)/sizeof(morse_hex_table[0]); ++i) {
        if (strcmp(ms->buf, morse_hex_table[i].morse) == 0) {
            int out = morse_hex_table[i].value;
            PORTC = options[out];
            overflow400 = 1;
            displaying = 1;
            return;
        } 
    }
    // not found
    PORTC = DECIMAL;
    overflow400 = 1;
    displaying = 1;
    ms_init(ms);
    return;
}

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//static unsigned int MAX_CLOCK = 1 << 19;
static unsigned int start_400ms = 65536 - 50000 - 5;//(1 << 15) - 400000/8;


static inline void tmr1_load_400ms(void) {
    TMR1 = start_400ms;  
}

static inline unsigned int get_mics() {
    return (TMR1 - start_400ms);
}

static void tmr1_init_400ms_tick(void) {
    // Timer1: Fosc/4, prescale 1:8
    T1CONbits.TMR1ON  = 0;    // stop while configuring
    T1CONbits.T1CKPS0 = 1;    // 1:8
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.TMR1CS  = 0;    // Fosc/4
    T1CONbits.T1OSCEN = 0;    // no T1 oscillator
    TMR1 = 0;
    tmr1_load_400ms();
    PIR1bits.TMR1IF = 0;      // clear flag
    PIE1bits.TMR1IE = 1;      // enable TMR1 interrupt
    INTCONbits.PEIE  = 1;     // enable peripheral interrupts
    T1CONbits.TMR1ON = 1;     // start
}

MorseStream my_stream;
void __interrupt() isr(void) 
{
    unsigned int mics = get_mics();
    TMR1 = 0;
    tmr1_load_400ms();  // FIXME: too long hold & error will break
    
    // ---- Timer1: millisecond tick ----
    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
//        tmr1_load_400ms();  // Reset the timer
        if (overflow400 > 5 && displaying==1) {  // Displayed for long enough, go back to idle
            PORTC = 0;
            displaying = 0;
            overflow400 = 0;  // time-out not started
            pressed = 0;
            ms_init(&my_stream);  // reset
        } else if (displaying == 1 && overflow400>0) {  // Increment timer on display
            overflow400++;
        } else if (overflow400 > 0 && pressed==1) {  // Time-out, took too long to release
            PORTC = DECIMAL & DASH; 
            displaying = 1;
            overflow400 = 1;
        } else if (overflow400 > 0) { 
            ms_display(&my_stream);
            displaying = 1;
        }
        PIR1bits.TMR1IF = 0;
    }
    if (INTCONbits.RBIE && INTCONbits.RBIF) {
        if (PORTBbits.RB1) {
            PORTC = 0;
            displaying = 0;
            overflow400 = 0;  // time-out not started
            pressed = 0;
            ms_init(&my_stream);  // reset
        } else if (displaying == 1) {}
        else if (PORTBbits.RB0 == 1) {  // Button Press (start timer)
            if (my_stream.len > 0 && mics <30000/8) {  // too short of a pause, ignore
                TMR1 += mics;
                return;
            }
            overflow400 = 1;  // time-out started
            pressed = 1;
        } else if (PORTBbits.RB0 == 0 && pressed==1) { // Button Release (capture timer)
            pressed = 0;
            overflow400 = 1;  // time-out started
            if (mics < 30000/8) {  // Didn't hold long enough
                PORTC = DASH;
                overflow400 = 1;
                displaying = 1;
            } else if (mics < 25000){//200000/8), Dot
//                PORTC = DECIMAL;
                ms_push_bit(&my_stream, 0);
            } else {  // Dash
//                PORTC = DASH;
                ms_push_bit(&my_stream, 1);
            }
        } 
        INTCONbits.RBIF = 0;   // clear IOC flag
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
    ms_init(&my_stream);
    tmr1_init_400ms_tick();
    
    PORTC = 0;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 1;  
    
    while(1) {
    }
    
}
/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "rtos.h"


// How to assign on-chip LEDs
// Format: <signalType> <var_name>(<pin_name>);
DigitalOut led1(LED1);
//TODO: define remaining 3 LED's


// Example to assign pins for as digital out
DigitalOut extern_pin1(p21); // sets pin21 as digital out
//TODO: define remaining external pins 


// Forward declarations (Part 5)
void Task1();
//TODO: declare remaining task function prototypes (will need 4 tasks)

// Forward declarations (Part 6)
void blink_on(DigitalOut* led, DigitalOut* ext_pin);
void blink_off(DigitalOut* led, DigitalOut* ext_pin);

//event queue
EventQueue queue;


//part 5
int main(void)
{
    //TODO: Add calls to the remaining tasks 
    queue.call_every(100ms,&Task1);    
    
    

    queue.dispatch_forever();
    
   return 0;
}

void Task1()
{
        //TODO: Add code to turn on the LED, sleep for a given amount of time (sleep the thread), then turn off the LED
}

//Define the remaining tasks as needed
void Task2()
{
        //TODO: Add code to turn on the LED, sleep for a given amount of time, then turn off the LED
}



// part 6

// void blink_on(DigitalOut* led, DigitalOut* ext_pin) {
//     //TODO: add code to turn on the LED and external pins

//     //TODO: use the queue.call_in function to call the blink_off after a given delay
//     queue.call_in(5ms, blink_off, led, ext_pin);  // schedule turning it off later
// }

// void blink_off(DigitalOut* led, DigitalOut* ext_pin) {
//     //TODO: add code to turn off the LEDs/pins
// }

// int main() {
//     //TODO: add code to call the blink_on for each LED at a given period
    


//     queue.dispatch_forever();
// }
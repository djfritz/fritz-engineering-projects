/* 
 * File:   main.c
 * Author: fritz
 *
 * Created on October 2, 2013, 8:12 PM
 */

// PIC16F1829 Configuration Bit Settings

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 500000 /* 500kHz default intosc */
#define LI RC0
#define EI RC1

#define MAX 1023
#define MIN 0
#define TICK 20
#define TIMEOUT_MIN 60
#define TIME_MAX (TIMEOUT_MIN*60000)

int limit = 0;

void WritePacket(int, unsigned int, unsigned int, unsigned int);
void Latch();

int main(int argc, char** argv) {
    ANSELA=0;
    ANSELB=0;
    ANSELC=0;
    TRISB = 0x0;
    TRISC = 0x0;
    PORTB = 0x0;
    PORTC = 0x0;
    //WPUC=0xff;

    //SPI
    SSPSTAT = 0x40;
    SSPCON1 = 0x22;

    srand(0);
    
    EI = 0;
    LI = 0;

    unsigned long timeout = 0;
    unsigned int red, green, blue, curr_red, curr_green, curr_blue;
    red = 0;
    green = 0;
    blue = 0;
    curr_red = 0;
    curr_green = 0;
    curr_blue = 0;
    
    while (1) {
        // pick a color to shift to:
        // red = 0
        // green = 1
        // blue = 2
        // yellow = 3
        // cyan = 4
        // magenta = 5
        // white = 6

        int color = rand() % 7;
        int rate_red, rate_green, rate_blue;
 

        switch (color) {
            case 0:
                red = MAX-500;
                green = MAX-300;
                blue = MAX;
                break;
            case 1:
                red = MAX;
                green = MIN;
                blue = MIN;
                break;
            case 2:
                red = MIN;
                green = MIN;
                blue = MAX;
                break;
            case 3:
                red = MAX;
                green = MAX-300;
                blue = MIN;
                break;
            case 4:
                red = MIN;
                green = MAX-300;
                blue = MAX;
                break;
            case 5:
                red = MAX-700;
                green = MIN;
                blue = MAX;
                break;
            case 6:
                red = MIN;
                green = MAX;
                blue = MIN;
                break;
        }

        if (timeout > TIME_MAX) {
            red = 0;
            green = 0;
            blue = 0;
        }

        // pick a random rate for each channel
        rate_red = (rand() % 7) + 1;
        rate_green = (rand() % 7) + 1;
        rate_blue = (rand() % 7) + 1;

        // change the rate direction if necessary
        if (red < curr_red) {
            rate_red *= -1;
        }
        if (green < curr_green) {
            rate_green *= -1;
        }
        if (blue < curr_blue) {
            rate_blue *= -1;
        }


        // start moving
        while (red != curr_red || green != curr_green || blue != curr_blue) {
            
            if (curr_red != red) {
                if (rate_red > 0 && red - curr_red < (unsigned int)rate_red) {
                    curr_red = red;
                } else if (rate_red < 0 && curr_red - red < (unsigned int)(rate_red*-1)) {
                    curr_red = red;
                } else {
                    curr_red += rate_red;
                }
            }

            if (curr_green != green) {
                if (rate_green > 0 && green - curr_green < (unsigned int)rate_green) {
                    curr_green = green;
                } else if (rate_green < 0 && curr_green - green < (unsigned int)(rate_green*-1)) {
                    curr_green = green;
                } else {
                    curr_green += rate_green;
                }
            }

            if (curr_blue != blue) {
                if (rate_blue > 0 && blue - curr_blue < (unsigned int)rate_blue) {
                    curr_blue = blue;
                } else if (rate_blue < 0 && curr_blue - blue < (unsigned int)(rate_blue*-1)) {
                    curr_blue = blue;
                } else {
                    curr_blue += rate_blue;
                }
            }

            if (limit < MAX ) {
                limit++;
                if (curr_red > limit) {
                    curr_red = limit;
                }
                if (curr_green > limit) {
                    curr_green = limit;
                }
                if (curr_blue > limit) {
                    curr_blue = limit;
                }
            }

            //WritePacket(1, 100, 100, 100);
            WritePacket(0, curr_red,curr_green,curr_blue);
           
            
            __delay_ms(TICK);
            timeout += TICK;
        }

        if (red == 0 && green == 0 && blue == 0 && timeout > TIME_MAX) {
            EI = 1;
            SLEEP(); 
        }

        __delay_ms(2000);
        timeout += 2000;
        
    }
    return (EXIT_SUCCESS);
}

void WritePacket(int command, unsigned int red, unsigned int green, unsigned int blue) {
    // first byte = 8'b00_blue[9:4]
    char byte0 = (char)(blue >> 4) & 0x3f;
    if (command == 1) {
        byte0 |= 0x40;
    }
    // second byte = 8'bblue[3:0]_red[9:6]
    char byte1 = (char)(blue << 4) | ((char)(red >> 6) & 0x0f);
    // third byte = 8'bred[5:0]_green[9:8]
    char byte2 = (char)(red << 2) | ((char)(green >> 8) & 0x03);
    // fourth byte = 8'bgreen[7:0]
    char byte3 = (char)(green);

    SSP1IF = 0;
    SSPBUF = byte0;
    while (SSP1IF == 0);
    SSP1IF = 0;
    __delay_us(100);
    SSPBUF = byte1;
    while (SSP1IF == 0);
    SSP1IF = 0;
    __delay_us(100);
    SSPBUF = byte2;
    while (SSP1IF == 0);
    SSP1IF = 0;
    __delay_us(100);
    SSPBUF = byte3;
    while (SSP1IF == 0);
    SSP1IF = 0;

    Latch();

    return;
}

void Latch() {
    __delay_us(100);
    LI = 1;
    //EI = 1;
    __delay_us(100);
    //EI = 0;
    LI = 0;
    //__delay_us(100);
}

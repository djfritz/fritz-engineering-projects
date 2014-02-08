/*
 * File:   main.c
 * Author: fritz
 *
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
#define BUZZER RB4
#define DOOR RB6
#define LED_HB RC0
#define LED_DOOR RC1

#define TICK 500

void interrupt intr(void) {
    if (RCIF) {
        // receive uart interrupt
        char data = RCREG;
        if (data == 1) {
            LED_DOOR = 1;
        } else if (data == 0) {
            LED_DOOR = 0;
        }
        LED_HB = !LED_HB;
    }
}

int main(int argc, char** argv) {
    ANSELA=0;
    ANSELB=0;
    ANSELC=0;
    TRISA = 0xff;
    TRISB = 0xef;
    TRISC = 0xfc;

    RXDTSEL = 0;
    TXCKSEL = 0;
    CREN = 1;
    SYNC = 0;
    SPEN = 1;
    BRG16 = 1;
    BRGH = 1;
    SPBRGH = 0;
    SPBRGL = 12;

    GIE = 1;
    RCIE = 1;
    PEIE = 1;

    LED_HB = 0;
    LED_DOOR = 0;

    while (1);
    return (EXIT_SUCCESS);
}

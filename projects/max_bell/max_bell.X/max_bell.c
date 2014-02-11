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
#define BUZZER RB4 /* buzzer is active low */
#define DOOR RB6
#define LED_HB RC0
#define LED_DOOR RC1

#define TICK 500
#define BUZZER_COUNT_MAX 20

int last = 0;
int buzzer_count = 0;

void interrupt intr(void) {
    if (RCIF) {
        RCIF = 0;
        // receive uart interrupt
        char data = RCREG;
        if (data == 1 && last == 0) {
            LED_DOOR = 1;
            BUZZER = 0;
            TMR0 = 100;
            TMR0IE = 1;
            TMR0IF = 0;
            last = 1;
        } else if (data == 2) {
            LED_DOOR = 0;
            last = 0;
        }
        /* normally i'd start a timer interrupt, but we know that the sending
         * side won't be sending another heartbeat for some time, so instead
         * i'll just make this a bit of a hack and actually sleep the amount
         * of time I need for the heartbeat indicator. */
        LED_HB = 1;
        __delay_ms(10);
        LED_HB = 0;
    } else if (TMR0IF) {
        BUZZER = !BUZZER;
        TMR0IF = 0;
        if (buzzer_count == BUZZER_COUNT_MAX) {
            TMR0IE = 0;
            BUZZER = 1;
            buzzer_count = 0;
        } else {
            buzzer_count++;
            TMR0 = 100;
        }
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

    TMR0CS = 0;
    PSA = 0;
    PS0 = 1;
    PS1 = 1;
    PS2 = 0;

    GIE = 1;
    RCIE = 1;
    PEIE = 1;

    LED_HB = 0;
    LED_DOOR = 0;
    BUZZER = 1;

    while (1);
    return (EXIT_SUCCESS);
}

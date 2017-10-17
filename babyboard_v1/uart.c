/* uart.c
 * Authors: Andrew Dudney and Matthew Haney
 * Copyright 2016 Andrew Dudney
 * Provides an implementation to the UART interface defined in uart.h
 */

//MSP430G2553

#include "msp430g2553.h"
#include "uart.h"

#define TXLED BIT0
#define RXLED BIT6
#define TXD BIT2
#define RXD BIT1
unsigned short ucbrx;

int counter = 0;
int clock_speed = 0;

void set_clock(int speed) {
    if (speed ==1 ){
        BCSCTL1 = CALBC1_1MHZ; // Set DCO
        DCOCTL = CALDCO_1MHZ;
        clock_speed = 1;
    }else if (speed == 8){
        BCSCTL1 = CALBC1_8MHZ; // Set DCO
        DCOCTL = CALDCO_8MHZ;
        clock_speed = 8;
    }else { //if (speed == 16){
        BCSCTL1 = CALBC1_16MHZ; // Set DCO
        DCOCTL = CALDCO_16MHZ;
        clock_speed = 16;
    }

}

void init_uart(char baud) {
   //Disable Port 2
   P2DIR = 0xFF; // All P2.x outputs<
   P2OUT &= 0x00; // All P2.x reset

   //Switch to UART mode for G2553
   P1SEL |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
   P1SEL2 |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD

   //Set up on board LEDs
   P1DIR |= RXLED + TXLED;
   P1OUT &= 0x00;

   //Select SMCLK as clock source for UART module
   UCA0CTL1 |= UCSSEL_2; // SMCLK

   //Select baud rate (All values are from page 424)
   if(baud==UART_BAUD_9600 && clock_speed == 1)
       ucbrx = 0x68; // 1MHz 9600
   else if(baud==UART_BAUD_9600 && clock_speed == 8)
       ucbrx = 0x341; // 8MHz 9600
   else if(baud==UART_BAUD_9600 && clock_speed == 16)
       ucbrx = 0x682; // 16MHz 9600
   else if(baud==UART_BAUD_115200 && clock_speed == 1)
       ucbrx = 0x08; // 1MHz 9600
   else if(baud==UART_BAUD_115200 && clock_speed == 8)
       ucbrx = 0x45; // 8MHz 9600
   else
       ucbrx = 0x8A; // 16MHz 115200

   UCA0BR0 = ucbrx & 0x00FF; // 1MHz 115200
   UCA0BR1 = (ucbrx & 0xFF00)>>8;
   UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5

   //Enable the UART module and the interrupt for the recieve event
   UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
   UC0IE |= !UCA0RXIE; // Enable USCI_A0 RX interrupt

   //Put microcontroller into low power mode and enable global interrupt flag

   //__bis_SR_register(CPUOFF + GIE); // Enter LPM0 w/ int until Byte RXed

}

void uninit_uart() {
    //Disable the UART module and the interrupt for the recieve event
    UCA0CTL1 |= UCSWRST; // **Initialize USCI state machine**
    UC0IE &= 0; // Enable USCI_A0 RX interrupt
}

void putch(unsigned char c) {
    IE2 |= UCA0TXIE;// Enable USCI_A0 TX interrupt
    UCA0TXBUF = c;
}

#pragma vector=USCIAB0TX_VECTOR //Vector for transmitting
void put_str(unsigned char* c) {
    int i = 0;
    do {
        putch(*(c + i));
        i++;
    }while(*(c + i) != '\n');
}

#pragma vector=USCIAB0RX_VECTOR //Vector for recieving
int uart_rx(char block) {
    if (block){
        while(!(UC0IFG & UCA0RXIFG));//Check if RX is busy
        return UCA0RXBUF;
    }
    else{
        if(UC0IFG & UCA0RXIFG){
            return UCA0RXBUF;
        }else
            return -1;

    }
}

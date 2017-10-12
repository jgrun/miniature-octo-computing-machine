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
int counter = 0;
int clock_speed = 0;

void set_clock(int speed) {
    if (speed ==1 ){
        BCSCTL1 = CALBC1_1MHZ; // Set DCO
        DCOCTL = CALDCO_1MHZ;
        speed = 1;
    }else if (speed == 8){
        BCSCTL1 = CALBC1_8MHZ; // Set DCO
        DCOCTL = CALDCO_8MHZ;
        speed = 8;
    }else { //if (speed == 16){
        BCSCTL1 = CALBC1_16MHZ; // Set DCO
        DCOCTL = CALDCO_16MHZ;
        speed = 16;
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
   if(baud==9600 && clock_speed == 1)
       UCA0BR0 = 0x68; // 1MHz 9600
   else if(baud==9600 && clock_speed == 8)
       UCA0BR0 = 0x341; // 8MHz 9600
   else if(baud==9600 && clock_speed == 16)
       UCA0BR0 = 0x682; // 16MHz 9600
   else if(baud==115200 && clock_speed == 1)
       UCA0BR0 = 0x08; // 1MHz 9600
   else if(baud==115200 && clock_speed == 8)
       UCA0BR0 = 0x45; // 8MHz 9600
   else
       UCA0BR0 = 0x8A; // 16MHz 115200

   UCA0BR1 = 0x00; // 1MHz 115200
   UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5

   //Enable the UART module and the interrupt for the recieve event
   UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
   UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt

   //Put microcontroller into low power mode and enable global interrupt flag
   __bis_SR_register(CPUOFF + GIE); // Enter LPM0 w/ int until Byte RXed
   //Enter infinite loop...while(1){}
}

void uninit_uart() {
  
}

void putch(unsigned char c) {
  
}

#pragma vector=USCIAB0TX_VECTOR //Vector for transmitting
void put_str(unsigned char* c) {
    P1OUT |= TXLED;             //Turn on LED so we know we are transmitting
    UCA0TXBUF = c[counter++];         // TX next character
    if (counter == sizeof c - 1)      // TX over?
       UC0IE &= ~UCA0TXIE;      // Disable USCI_A0 TX interrupt
    P1OUT &= ~TXLED;
}

#pragma vector=USCIAB0RX_VECTOR //Vector for recieving
int uart_rx(char block) {
  //Stuff!
    return -1;
}

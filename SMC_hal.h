/***********************************************************************************
Author      :     Laurent Folladore, Bertrand Hochet
Date        :     07.10.2014
Filename    :     SMC_hal.h

uC          :     MSP430 family f5xx
C compiler  :     CCS

object      :     Labos SMC et SMCO

Copyright   :     HEIG-VD

************************************************************************************/

#ifndef __SMC_hal__
#define __SMC_hal__

// NAME      PORT  MSP_DIR  FUNC
//-----------------------------------

// SW1       P1.7  in       normal
// SW2       P2.2  in       normal
// LED1      P1.0  out      normal
// LED2      P8.1  out      normal
// LED3      P8.3  out      normal
// PAD1      P1.1  out      normal
// PAD2      P1.2  out      normal
// PAD3      P1.3  out      normal
// PAD4      P1.4  out      normal
// PAD5      P1.5  out      normal

#define SW1_BIT 1
#define SW1_DIR P1DIR
#define SW1_OUT P1OUT
#define SW1_IN  P1IN
#define SW1_SEL P1SEL
#define SW1_REN P1REN
#define SET_SW1_DIR() {SW1_DIR |= (1 << SW1_BIT);}
#define CLR_SW1_DIR() {SW1_DIR &= ~(1 << SW1_BIT);}
#define SET_SW1_OUT() {SW1_OUT |= (1 << SW1_BIT);} 
#define CLR_SW1_OUT() {SW1_OUT &= ~(1 << SW1_BIT);}
#define GET_SW1_IN()  (SW1_IN & (1 << SW1_BIT))		// expression simple, pouvant être utilisée dans un test
#define SET_SW1_SEL() {SW1_SEL |= (1 << SW1_BIT);} 
#define CLR_SW1_SEL() {SW1_SEL &= ~(1 << SW1_BIT);}
#define SET_SW1_REN() {SW1_REN |= (1 << SW1_BIT);} 
#define CLR_SW1_REN() {SW1_REN &= ~(1 << SW1_BIT);}

#define SW2_BIT 2
#define SW2_DIR P2DIR
#define SW2_OUT P2OUT
#define SW2_IN  P2IN
#define SW2_SEL P2SEL
#define SW2_REN P2REN
#define SET_SW2_DIR() {SW2_DIR |= (1 << SW2_BIT);} 
#define CLR_SW2_DIR() {SW2_DIR &= ~(1 << SW2_BIT);}
#define SET_SW2_OUT() {SW2_OUT |= (1 << SW2_BIT);} 
#define CLR_SW2_OUT() {SW2_OUT &= ~(1 << SW2_BIT);}
#define GET_SW2_IN()  (SW2_IN & (1 << SW2_BIT))    
#define SET_SW2_SEL() {SW2_SEL |= (1 << SW2_BIT);} 
#define CLR_SW2_SEL() {SW2_SEL &= ~(1 << SW2_BIT);}
#define SET_SW2_REN() {SW2_REN |= (1 << SW2_BIT);} 
#define CLR_SW2_REN() {SW2_REN &= ~(1 << SW2_BIT);}


#define LED1_BIT 0
#define LED1_DIR P1DIR
#define LED1_OUT P1OUT
#define SET_LED1_DIR() {LED1_DIR |= (1 << LED1_BIT);} 
#define SET_LED1()     {LED1_OUT |= (1 << LED1_BIT);} 
#define CLR_LED1()     {LED1_OUT &= ~(1 << LED1_BIT);}

#define LED2_BIT 1
#define LED2_DIR P8DIR
#define LED2_OUT P8OUT
#define SET_LED2_DIR() {LED2_DIR |= (1 << LED2_BIT);} 
#define SET_LED2()     {LED2_OUT |= (1 << LED2_BIT);} 
#define CLR_LED2()     {LED2_OUT &= ~(1 << LED2_BIT);}

#define LED3_BIT 3
#define LED3_DIR P8DIR
#define LED3_OUT P8OUT
#define SET_LED3_DIR() {LED3_DIR |= (1 << LED3_BIT);} 
#define SET_LED3()     {LED3_OUT |= (1 << LED3_BIT);} 
#define CLR_LED3()     {LED3_OUT &= ~(1 << LED3_BIT);}

/* définitions des LEDs PADx */
/* A faire : configurations pour touchpads */

#define PAD1_BIT 1
#define PAD1_DIR P1DIR
#define PAD1_OUT P1OUT
#define SET_PAD1_DIR() {PAD1_DIR |= (1 << PAD1_BIT);} 
#define SET_PAD1()     {PAD1_OUT |= (1 << PAD1_BIT);} 
#define CLR_PAD1()     {PAD1_OUT &= ~(1 << PAD1_BIT);}
#define TGL_PAD1()     {PAD1_OUT ^= (1 << PAD1_BIT);}

#define PAD2_BIT 2
#define PAD2_DIR P1DIR
#define PAD2_OUT P1OUT
#define SET_PAD2_DIR() {PAD2_DIR |= (1 << PAD2_BIT);} 
#define SET_PAD2()     {PAD2_OUT |= (1 << PAD2_BIT);} 
#define CLR_PAD2()     {PAD2_OUT &= ~(1 << PAD2_BIT);}
#define TGL_PAD2()     {PAD2_OUT ^= (1 << PAD2_BIT);}

#define PAD3_BIT 3
#define PAD3_DIR P1DIR
#define PAD3_OUT P1OUT
#define SET_PAD3_DIR() {PAD3_DIR |= (1 << PAD3_BIT);} 
#define SET_PAD3()     {PAD3_OUT |= (1 << PAD3_BIT);} 
#define CLR_PAD3()     {PAD3_OUT &= ~(1 << PAD3_BIT);}
#define TGL_PAD3()     {PAD3_OUT ^= (1 << PAD3_BIT);}

#define PAD4_BIT 4
#define PAD4_DIR P1DIR
#define PAD4_OUT P1OUT
#define SET_PAD4_DIR() {PAD4_DIR |= (1 << PAD4_BIT);} 
#define SET_PAD4()     {PAD4_OUT |= (1 << PAD4_BIT);} 
#define CLR_PAD4()     {PAD4_OUT &= ~(1 << PAD4_BIT);}
#define TGL_PAD4()     {PAD4_OUT ^= (1 << PAD4_BIT);}

#define PAD5_BIT 5
#define PAD5_DIR P1DIR
#define PAD5_OUT P1OUT
#define SET_PAD5_DIR() {PAD5_DIR |= (1 << PAD5_BIT);} 
#define SET_PAD5()     {PAD5_OUT |= (1 << PAD5_BIT);} 
#define CLR_PAD5()     {PAD5_OUT &= ~(1 << PAD5_BIT);}
#define TGL_PAD5()     {PAD5_OUT ^= (1 << PAD5_BIT);}


#endif // __SMC_hal__

/*** UART Driver for PIC24 ***/
#include "p24fxxxx.h"
// External oscillator frequency
#define SYSCLK          8000000

// UART IOs
#define UART2_TX_TRIS   TRISFbits.TRISF5
#define UART2_RX_TRIS   TRISFbits.TRISF4

/*****************************************************************************
 * DEFINITIONS
 *****************************************************************************/
// Baudrate
#define BAUDRATE2		19200
/*****************************************************************************
 * U2BRG register value and baudrate mistake calculation
 *****************************************************************************/
#define BAUDRATEREG2 SYSCLK/32/BAUDRATE2-1

#if BAUDRATEREG2 > 255
#error Cannot set up UART2 for the SYSCLK and BAUDRATE.\
 Correct values in main.h and uart2.h files.
#endif

#define BAUDRATE_MISTAKE 1000*(BAUDRATE2-SYSCLK/32/(BAUDRATEREG2+1))/BAUDRATE2
#if (BAUDRATE_MISTAKE > 2)||(BAUDRATE_MISTAKE < -2)
#error UART2 baudrate mistake is too big  for the SYSCLK\
 and BAUDRATE2. Correct values in uart2.c file.
#endif 
extern void InitUART2(void);
extern void  UART2PutChar(char );
/*UART2Init must be called before. converts decimal data into a string**/
extern void  UART2PutDec(unsigned char );


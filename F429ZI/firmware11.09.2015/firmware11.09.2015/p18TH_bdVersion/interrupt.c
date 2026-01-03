/*** interrupt.c ***/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include <timers.h>
#include "typedefs.h"
/** V A R I A B L E S ********************************************************/
extern unsigned char received[];
extern unsigned char fMess;
extern unsigned char myAdd;
unsigned int nRec = 0;
unsigned int expected;
/** P R I V A T E  P R O T O T Y P E S ***************************************/
void high_isr(void);
void low_isr(void);
/** I N T E R R U P T  V E C T O R S *****************************************/

#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
    _asm goto high_isr _endasm
}
#pragma code

#pragma code low_vector=0x18
void interrupt_at_low_vector(void)
{
    _asm goto low_isr _endasm
}
#pragma code

/** D E C L A R A T I O N S **************************************************/
/*** Function:        void high_isr(void) ***/
#pragma interrupt high_isr
void high_isr(void) {

    //INTCONbits.RBIF = 0;
    if(PIR1bits.SSPIF)	{       // if interrupt from SPI (shouldn't happen)
        PIR1bits.SSPIF = 0;     //Clear interrupt flag
    }
    if (PIR1bits.RCIF == 1) {   // if interrupt from usart
        received[nRec] = ReadUSART();   // Get char from USART
        switch (nRec) {
            case 0:                     // first character must be STX
                if (received[0] == 2) nRec++;
                break;
            case 1:
                nRec++;
                expected = received[1];
                break;
            case 11:                    // last character must be ETX
                nRec++;
                if (nRec == expected && received[nRec-1] == 3) {
                    nRec = 0;
                    if (received[2] == myAdd)
                        fMess = TRUE;
                }
                break;
            default:
                nRec++;
        }
        if ((nRec > 2) && (nRec >= expected))
            nRec = 0;
        PIR1bits.RCIF = 0;              /* Clear the interrupt flag */
    }
}
#pragma code
/******************************************************************************
 * Function:        void low_isr(void)
 *****************************************************************************/
#pragma interruptlow low_isr
void low_isr(void) {
        if (INTCONbits.TMR0IF)
            INTCONbits.TMR0IF = 0;
}

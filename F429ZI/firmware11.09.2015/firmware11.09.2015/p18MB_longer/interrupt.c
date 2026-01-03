/*** interrupt.c ***/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include <timers.h>
#include "typedefs.h"
//#include "interrupt.h"

/** V A R I A B L E S ********************************************************/
//extern BOOL gSampleFlag;                // not used
//extern BOOL AcquireData;                // not used
//extern BOOL recFlag;                    // not used
//extern unsigned int TimerCounterDJ;     // not used
extern unsigned char received[];
extern unsigned char mymessage[];
extern unsigned char fMmess;
extern unsigned char fSmess;
extern unsigned char myAdd;
unsigned int expected;
unsigned int nRec = 0;
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
/*** Interrupt: accepts only messages of 12 and 38 characters              ***/
#pragma interrupt high_isr
void high_isr(void) {
    unsigned int i;
    //INTCONbits.RBIF = 0;
    if(PIR1bits.SSPIF)	{               // if interrupt from SPI (shouldn't happen)
        PIR1bits.SSPIF = 0;             //Clear interrupt flag
    }
    if (PIR1bits.RCIF == 1) {           // if interrupt from usart
        received[nRec] = ReadUSART();   // Get char from USART
        switch (nRec) {
            case 0:
                if (received[0] == 2) nRec++;
                break;
            case 1:
                nRec++;
                expected = received[1];
                break;
            case 11:
                nRec++;
 //               if (expected != 38 && expected != 12)
   //                 expected = 12;
                if (nRec == expected && received[nRec-1] == 3) {
                    nRec = 0;   
                    if (received[2] == myAdd)
                        fSmess = TRUE;
                }
                break;
            case 37:
                nRec++;
                if (expected == nRec && received[nRec-1] == 3) {
                    nRec = 0;
                    fMmess = TRUE;
                    for (i=0; i<33; i++)    // copy selection and flagLastCh
                        mymessage[i]=received[i+4];
                }
                break;
            default:
                nRec++;
        }
        if ((nRec > 2) && (nRec >= expected))
            nRec = 0;
        PIR1bits.RCIF = 0;              /* Clear the interrupt flag */
    }
/*    if (INTCONbits.TMR0IF){
        TimerCounterDJ--;
        if (TimerCounterDJ==0){         //if rolled over, set flag
            TimerCounterDJ = 3;         // 1000 = 1 sec
            gSampleFlag = 1;
            WriteTimer0(250);           // 1 msec for timer
        }   
        INTCONbits.TMR0IF = 0;
    } */
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

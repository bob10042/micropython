/**********************************************************************************************************************
* Triteq Ltd. The Innovation Centre, Station Road, Hungerford, Berkshire. RG17 0DY
* Tel: +44 (0)1488 684554     Fax: +44 (0)1488 685335    www.triteq.com
* Copyright 2015 Triteq Ltd. All Rights Reserved.
***********************************************************************************************************************
*
*       Project:    MM20        Name:   Bias PSU
*
***********************************************************************************************************************/
/*!     @file       interrupts.c
*       @brief      Interrupt Handlers
*                   high and low isr handlers
***********************************************************************************************************************
<pre>   @version
        Version     Date            Author      Description
        1.0.0       22/04/2015      Guy Mundy   Initial Release
</pre>
***********************************************************************************************************************/

/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#include <p18cxxx.h>    /* C18 General Include File */

#include <usart.h>
#include "main.h"
#include "typedefs.h"

/******************************************************************************/
/* Static Variable Declaration                                           */
/******************************************************************************/
static unsigned int nRec = 0;
static unsigned int expected;

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

#pragma interrupt high_isr

void    high_isr(void)
{
    if(PIR1bits.SSPIF)
    {       // if interrupt from SPI (shouldn't happen)
        PIR1bits.SSPIF = 0;     //Clear interrupt flag
    }

    if (PIR1bits.RCIF == 1)
    {   // if interrupt from usart
        received[nRec] = Read1USART();   // Get char from USART
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
        {
            nRec = 0;
        }

        PIR1bits.RCIF = 0;              /* Clear the interrupt flag */
    }
}

#pragma interruptlow low_isr

void    low_isr(void)
{
    if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
    }
}

#pragma code high_vector=0x08

void    high_vector(void)
{
   _asm GOTO high_isr _endasm
}

#pragma code low_vector=0x18

void    low_vector(void)
{
   _asm GOTO low_isr _endasm
}

#pragma code


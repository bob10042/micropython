/**********************************************************************************************************************
* Triteq Ltd. The Innovation Centre, Station Road, Hungerford, Berkshire. RG17 0DY
* Tel: +44 (0)1488 684554     Fax: +44 (0)1488 685335    www.triteq.com
* Copyright 2015 Triteq Ltd. All Rights Reserved.
***********************************************************************************************************************
*
*       Project:    MM20        Name:   Bias PSU
*
***********************************************************************************************************************/
/*!     @file       main.c
*       @brief      Main file
*                   Initialisation and command dispatcher
***********************************************************************************************************************
<pre>   @version
        Version     Date            Author      Description
        1.0.0       22/04/2015      Guy Mundy   Initial Release
</pre>
***********************************************************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <p18cxxx.h>   /* C18 General Include File */

#include <usart.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "main.h"
#include "typedefs.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
//#define boardNO     17
//#define nMessLength 12
//#define POSITIVE    1
//#define NEGATIVE    0

unsigned char fMess = FALSE;
unsigned char mess2p24[nMessLength];
unsigned char received[64];
unsigned char myAdd;
static char ver[] = "P10201";
unsigned char Bias_1_Polarity, Bias_2_Polarity, Bias_3_Polarity;


/******************************************************************************/
/* Send result to pic24                                                       */
/******************************************************************************/
void send12Data(unsigned char res[])
{
    int nn, j;

    EN_485 = 1;
    for (nn=0;nn<300;nn++);             // min 150
    for (j=0;j<12;j++)
    {
        Write1USART((char)res[j]);        //send 12 bytes
        for (nn=0;nn<300;nn++);         // min 200
    }
    //for (nn=0;nn<200;nn++);
    EN_485 = 0;
    for (nn=0;nn<200;nn++);
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    unsigned int i, j, nAvg;
    unsigned char * pch;
    unsigned adc_result;
    unsigned tmp;

    myAdd = boardNO;
    nAvg = 1;                       // default: no averaging

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    Init_Ports();
    Init_ADC();
    Init_Timer();
    Init_SPI_1();

    //Enable interrupts
    INTCONbits.PEIE = 0;                // Turn on peripheral interrupts
    INTCONbits.GIE = 0;			// Turn on global interrupts

    Init_USART_1();

    while(1)
    {
        if (fMess) {
            fMess = FALSE;          // message arrived
            switch (received[4]) {           
                case 0x7F: /* request for version */
                    mess2p24[4] = ver[0];
                    mess2p24[5] = ver[1];
                    mess2p24[6] = ver[2];
                    mess2p24[7] = ver[3];
                    mess2p24[8] = ver[4];
                    mess2p24[9] = ver[5];
                    break;
                default: /* nothing to do */
                    break;
            } /* switch (received[4]) */
            send12Data(mess2p24);
        } /* if (fMess) */
    }  /* while(1) */
} /* main() */
/* EOF */

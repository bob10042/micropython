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
static char ver[] = "P10202";
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
        Write1USART((char)res[j]);      // send 12 bytes
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
        if (fMess)
        {
            fMess = FALSE;          // message arrived
            switch (received[4])
            {           
                case 0x00: /* GND-GND-GND */
                    GND_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    GND_Bias(BIAS3);
                   break;
                case 0x01: /* INT-50-INT-50-INT-50 */
                    Bias1_Voltage(V50_NEG);
                    Bias2_Voltage(V50_NEG);
                    Bias3_Voltage(V50_NEG);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    INT_HV_Bias(BIAS3);
                    break;
                case 0x02: /* INT50-INT50-INT50 */
                    Bias1_Voltage(V50_POS);
                    Bias2_Voltage(V50_POS);
                    Bias3_Voltage(V50_POS);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    INT_HV_Bias(BIAS3);
                    break;
                case 0x03: /* OC-OC-OC */
                    OC_Bias(BIAS1);
                    OC_Bias(BIAS2);
                    OC_Bias(BIAS3);
                    break;
                case 0x04: /* INT5-INT5-INT5*/
                    Bias1_Voltage(V5);
                    Bias2_Voltage(V5);
                    Bias3_Voltage(V5);
                    INT_LV_Bias(BIAS1);
                    INT_LV_Bias(BIAS2);
                    INT_LV_Bias(BIAS3);
                    break;
                case 0x05: /* INT10-INT10-INT10*/
                    Bias1_Voltage(V10);
                    Bias2_Voltage(V10);
                    Bias3_Voltage(V10);
                    INT_LV_Bias(BIAS1);
                    INT_LV_Bias(BIAS2);
                    INT_LV_Bias(BIAS3);
                    break;
                case 0x06: /* INT100-INT100-INT100 */
                    Bias1_Voltage(V100_POS);
                    Bias2_Voltage(V100_POS);
                    Bias3_Voltage(V100_POS);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    INT_HV_Bias(BIAS3);
                    break;
                case 0x07: /* INT-100-INT-100-INT-100 */
                    Bias1_Voltage(V100_NEG);
                    Bias2_Voltage(V100_NEG);
                    Bias3_Voltage(V100_NEG);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    INT_HV_Bias(BIAS3);
                    break;
                case 0x08: /* EXTV1-EXTV1-EXTV1 */
                    EXTV1_Bias(BIAS1);
                    EXTV1_Bias(BIAS2);
                    EXTV1_Bias(BIAS3);
                   break;
                case 0x09: /* EXTV2-EXTV2-EXTV2 */
                    EXTV2_Bias(BIAS1);
                    EXTV2_Bias(BIAS2);
                    EXTV2_Bias(BIAS3);
                   break;
                case 0x0A: /* GND-*-* */
                    GND_Bias(BIAS1);
                   break;
                case 0x0B: /* INT-50-*-* */
                    Bias1_Voltage(V50_NEG);
                    INT_HV_Bias(BIAS1);
                    break;
                case 0x0C: /* INT50-*-* */
                    Bias1_Voltage(V50_POS);
                    INT_HV_Bias(BIAS1);
                    break;
                case 0x0D: /* OC-*-* */
                    OC_Bias(BIAS1);
                    break;
                case 0x0E: /* INT5-*-* */
                    Bias1_Voltage(V5);
                    INT_LV_Bias(BIAS1);
                    break;
                case 0x0F: /* INT10-*-**/
                    Bias1_Voltage(V10);
                    INT_LV_Bias(BIAS1);
                    break;
                case 0x10: /* INT100-*-* */
                    Bias1_Voltage(V100_POS);
                    INT_HV_Bias(BIAS1);
                    break;
                case 0x11: /* INT-100-*-* */
                    Bias1_Voltage(V100_NEG);
                    INT_HV_Bias(BIAS1);
                    break;
                case 0x12: /* EXTV1-*-* */
                    EXTV1_Bias(BIAS1);
                    break;
                case 0x13: /* EXTV2-*-* */
                    EXTV2_Bias(BIAS1);
                    break;
                case 0x14: /* GND-GND-* */
                    GND_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    break;
                case 0x15: /* INT-50-INT-50-* */
                    Bias1_Voltage(V50_NEG);
                    Bias2_Voltage(V50_NEG);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    break;
                case 0x16: /* INT50-INT50-* */
                    Bias1_Voltage(V50_POS);
                    Bias2_Voltage(V50_POS);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    break;
                case 0x17:
                     /* OC-OC-* */
                    OC_Bias(BIAS1);
                    OC_Bias(BIAS2);
                    break;
                case 0x18: /* INT5-INT5-* */
                    Bias1_Voltage(V5);
                    Bias2_Voltage(V5);
                    INT_LV_Bias(BIAS1);
                    INT_LV_Bias(BIAS2);
                    break;
                case 0x19:
                     /* INT10-INT10-* */
                    Bias1_Voltage(V10);
                    Bias2_Voltage(V10);
                    INT_LV_Bias(BIAS1);
                    INT_LV_Bias(BIAS2);
                    break;
                case 0x1A: /* INT100-INT100-* */
                    Bias1_Voltage(V100_POS);
                    Bias2_Voltage(V100_POS);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    break;
                case 0x1B: /* INT-100-INT-100-* */
                    Bias1_Voltage(V100_NEG);
                    Bias2_Voltage(V100_NEG);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    break;
                case 0x1C: /* EXTV1-EXTV1-* */
                    EXTV1_Bias(BIAS1);
                    EXTV1_Bias(BIAS2);
                    break;
                case 0x1D: /* EXTV2-EXTV2-* */
                    EXTV2_Bias(BIAS1);
                    EXTV2_Bias(BIAS2);
                    break;
                case 0x1F: /* GND-INT5-**/
                    GND_Bias(BIAS1);
                    Bias2_Voltage(V5);
                    INT_LV_Bias(BIAS2);
                    break;
                case 0x20: /* INT50-GND-* */
                    Bias1_Voltage(V50_POS);
                    INT_HV_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    break;
                case 0x22: /* INT5-GND-**/
                    Bias1_Voltage(V5);
                    INT_LV_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    break;
                case 0x23: /* GND-INT50-* */
                    GND_Bias(BIAS1);
                    Bias2_Voltage(V50_POS);
                    INT_HV_Bias(BIAS2);
                    break;
                case 0x24: /* INT100-GND-* */
                    Bias1_Voltage(V100_POS);
                    INT_HV_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    break;
                case 0x25: /* GND-INT100-* */
                    GND_Bias(BIAS1);
                    Bias2_Voltage(V100_POS);
                    INT_HV_Bias(BIAS2);
                    break;
                case 0x26: /* GND-EXTV2-* */
                    GND_Bias(BIAS1);
                    EXTV2_Bias(BIAS2);
                    break;
                case 0x27: /* EXTV2-GND-* */
                    EXTV2_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    break;
                case 0x2A: /* INT50-INT50-GND */
                    Bias1_Voltage(V50_POS);
                    Bias2_Voltage(V50_POS);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    GND_Bias(BIAS3);
                    break;
                case 0x2D: /* INT50-GND-INT50 */
                    Bias1_Voltage(V50_POS);
                    Bias3_Voltage(V50_POS);
                    GND_Bias(BIAS2);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS3);
                    break;
                case 0x2E: /* INT100-INT100-GND */
                    Bias1_Voltage(V100_POS);
                    Bias2_Voltage(V100_POS);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS2);
                    GND_Bias(BIAS3);
                    break;
                case 0x2F: /* INT100-GND-INT100 */
                    Bias1_Voltage(V100_POS);
                    Bias3_Voltage(V100_POS);
                    GND_Bias(BIAS2);
                    INT_HV_Bias(BIAS1);
                    INT_HV_Bias(BIAS3);
                    break;
                case 0x30: /* EXTV2-GND-EXTV2 */
                    EXTV2_Bias(BIAS1);
                    GND_Bias(BIAS2);
                    EXTV2_Bias(BIAS3);
                    break;
                case 0x31: /* EXTV2-EXTV2-GND */
                    EXTV2_Bias(BIAS1);
                    EXTV2_Bias(BIAS2);
                    GND_Bias(BIAS3);
                    break;
                case 0x32: /* INT3-INT3-INT3 */
                    Bias1_Voltage(V3V3);
                    Bias2_Voltage(V3V3);
                    Bias3_Voltage(V3V3);
                    INT_LV_Bias(BIAS1);
                    INT_LV_Bias(BIAS2);
                    INT_LV_Bias(BIAS3);
                    break;
                case 0x7E: /* Request to measure */
                    if (received[5] > 0) nAvg = (unsigned)received[5];
                    pch = (unsigned char *)&(tmp);
                    tmp = 0;

                    Select_Voltage_Monitor(BIAS1);
                    for (j=0;j<1000;j++);
                    for (i=0; i<nAvg; i++) {
                        if (Bias_1_Polarity == NEGATIVE)
                        {
                            ADCON0 = BIAS_VM1_N_CH;
                        }
                        else
                        {
                            ADCON0 = BIAS_VM1_P_CH;
                        }
                        ADCON0 |= 0x03;
                        while (ADCON0bits.GO == 1); //done?
                        adc_result = ADRES;
                        tmp += adc_result;
                        for (j=0;j<10;j++);
                    }
                    tmp /= nAvg;                // averaged

                    mess2p24[4]= *(pch+1);
                    mess2p24[5] = *pch;

                    for (j=0;j<2000;j++);

                    tmp = 0;
                    Select_Voltage_Monitor(BIAS2);
                    for (j=0;j<1000;j++);

                    for (i=0; i<nAvg; i++) {
                        if (Bias_2_Polarity == NEGATIVE)
                        {
                            ADCON0 =  BIAS_VM2_N_CH;
                        }
                        else
                        {
                            ADCON0 =  BIAS_VM2_P_CH;
                        }
                        ADCON0 |= 0x03;
                        while (ADCON0bits.GO == 1); //done?
                        adc_result = ADRES;
                        tmp += adc_result;
                        for (j=0;j<10;j++);
                    }
                    tmp /= nAvg;                // averaged

                    mess2p24[6]= *(pch+1);
                    mess2p24[7] = *pch;

                    for (j=0;j<2000;j++);

                    tmp = 0;
                    Select_Voltage_Monitor(BIAS3);
                    for (j=0;j<1000;j++);

                    for (i=0; i<nAvg; i++) {
                        if (Bias_3_Polarity == NEGATIVE)
                        {
                            ADCON0 = BIAS_VM3_N_CH;
                        }
                        else
                        {
                            ADCON0 = BIAS_VM3_P_CH;
                        }
                        ADCON0 |= 0x03;
                        while (ADCON0bits.GO == 1); //done?
                        adc_result = ADRES;
                        tmp += adc_result;
                        for (j=0;j<10;j++);
                    }
                    tmp /= nAvg;                // averaged
                    mess2p24[8]= *(pch+1);
                    mess2p24[9] = *pch;
                    break;
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
            
            if (received[4] != 126)
            {
                mess2p24[4] = ver[0];   //version request
                mess2p24[5] = ver[1];
                mess2p24[6] = ver[2];
                mess2p24[7] = ver[3];
                mess2p24[8] = ver[4];
                mess2p24[9] = ver[5];
            }
            send12Data(mess2p24);
                
        } /* if (fMess) */
 
    }  /* while(1) */

} /* main() */

/* EOF */

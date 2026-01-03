/**********************************************************************************************************************
* Triteq Ltd. The Innovation Centre, Station Road, Hungerford, Berkshire. RG17 0DY
* Tel: +44 (0)1488 684554     Fax: +44 (0)1488 685335    www.triteq.com
* Copyright 2015 Triteq Ltd. All Rights Reserved.
***********************************************************************************************************************
*
*       Project:    MM20        Name:   Bias PSU
*
***********************************************************************************************************************/
/*!     @file       user.c
*       @brief      Hardware driver functions
*       @note       Specific for PCB MM20-TRI-PCB-1408_PSU-003 (revision 003 ECR 010)
*
***********************************************************************************************************************
<pre>   @version
        Version     Date            Author      Description
        1.0.0       22/04/2015      Guy Mundy   Initial Release
        1.0.1       14/05/2015      Guy Mundy   Updated to handle negative biases on measurement system.
</pre>
***********************************************************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <p18cxxx.h>    /* C18 General Include File */

#include <timers.h>
#include <usart.h>

#include "user.h"
#include "main.h"
#include "typedefs.h"

static Bias_V_t  Bias1_VM, Bias2_VM, Bias3_VM;

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

static void Set_Voltage(uint16_t RDAC);

void Init_ADC(void)
{
    ADCON0 = 0;
    ADCON1 = 0xb8;                      //right justified; 20 Tad, Fosc/2, Vref=3.3V
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA5 = 0;
    TRISFbits.TRISF2 = 0; /* for now we will make them +ve outputs to bias against dual polarity operation */
    ADCON0bits.ADON = 1;
}

void Init_Ports(void)
{
    PORTA = 0xFF; /* bias analogue high */
    TRISA = 0x00; /* bias analogue high */
    PORTB = 0x20;
    TRISB = 0xC0;
    PORTC = 0x0F;
    TRISC = 0x90;
    PORTD = 0x20;
    TRISD = 0xC0;
    PORTE = 0x20;
    TRISE = 0x00;
    PORTF = 0xE4; /* bias analogue high */
    WDTCONbits.ADSHR = 1; /* allow shared SFR */
    ANCON1 = 0x0C; /* make RF 5,6 digital */
    WDTCONbits.ADSHR = 0;
    TRISF = 0x1D; /* bias analogue high */

    /* set polarities to match */
    Bias_1_Polarity = POSITIVE;
    Bias_2_Polarity = POSITIVE;
    Bias_3_Polarity = POSITIVE;
}

void Init_Timer(void)
{
    TMR0H = 0;				// clear timer0
    TMR0L = 0;				// clear timer0
    T0CONbits.PSA = 0;			// Assign prescaler to Timer 0
    T0CONbits.T0PS2 = 1;		// Setup prescaler
    T0CONbits.T0PS1 = 1;		// Will time out every 51 us based on
    T0CONbits.T0PS0 = 1;		// 20 MHz Fosc
    T0CONbits.T0CS = 0;			// Increment on instuction cycle

    OpenTimer0 (TIMER_INT_OFF & T0_SOURCE_INT & T0_8BIT & T0_PS_1_1);
}

void Init_SPI_1(void)
{
    uint8_t temp;

    SSP1STAT &= 0x3F;               // power on state
    SSP1CON1 = 0x00;                // power on state
    SSP1CON1 |= 0x01;          // select serial mode Fosc/16

    SSP1STATbits.CKE = 0;       // data transmitted on rising edge
    SSP1CON1bits.CKP = 0;       // clock idle state low

    TRISCbits.TRISC3 = 0;       // define clock pin as output

    TRISCbits.TRISC4 = 1;       // define SDI pin as input
    TRISCbits.TRISC5 = 0;       // define SDO pin as output

    SSP1CON1 |= 0x20;             	// enable synchronous serial port

    /* Disable write protection */
    CS_LV_1 = 0; /* SYNC pulse */
    temp=SSP1BUF;
    SSP1BUF = 0x18;     /* write SPI */
    while (SSP1STATbits.BF == 0) {};
    temp=SSP1BUF;
    SSP1BUF  = 0x02;      /* 1% performance, write enable */
    while (SSP1STATbits.BF == 0) {};
    CS_LV_1 = 1;

    CS_LV_2 = 0; /* SYNC pulse */
    temp=SSP1BUF;
    SSP1BUF = 0x18;     /* write SPI */
    while (SSP1STATbits.BF == 0) {};
    temp=SSP1BUF;
    SSP1BUF  = 0x02;      /* 1% performance, write enable */
    while (SSP1STATbits.BF == 0) {};
    CS_LV_2 = 1;

    CS_LV_3 = 0; /* SYNC pulse */
    temp=SSP1BUF;
    SSP1BUF = 0x18;     /* write SPI */
    while (SSP1STATbits.BF == 0) {};
    temp=SSP1BUF;
    SSP1BUF  = 0x02;      /* 1% performance, write enable */
    while (SSP1STATbits.BF == 0) {};
    CS_LV_3 = 1;

    PIE1bits.SSPIE = 0;			//Turn off peripheral interrupt
    PIR1bits.SSPIF = 0;			//Clear any pending interrupt
}

void Init_USART_1(void)
{
    mess2p24[0]=2;
    mess2p24[1]=12;
    mess2p24[2]=100;
    mess2p24[3]=myAdd;
    mess2p24[4]=60;
    mess2p24[5]=60;
    mess2p24[6]=60;
    mess2p24[7]=60;
    mess2p24[8]=60;
    mess2p24[9]=60;
    mess2p24[10]=0;
    mess2p24[11]=3;

    Open1USART (USART_TX_INT_OFF &   // Open the USART: 8N1,19200 baud
             USART_RX_INT_ON &
             USART_ASYNCH_MODE &
             USART_EIGHT_BIT &
             USART_CONT_RX &
             USART_BRGH_HIGH,64);  /* BAUDCON 0, 9600 baud, Osc 20MHz and BRGH 1 */
    /////////////inserted
//    Open1USART (USART_TX_INT_OFF &   // Open the USART: 8N1,19200 baud
//             USART_RX_INT_ON &
//             USART_ASYNCH_MODE &
//             USART_EIGHT_BIT &
//             USART_CONT_RX &
//             USART_BRGH_HIGH,25); /* BAUDCON 0, 9600 baud, Osc 20MHz and BRGH 1 */
//    BAUDCON = 0x08;
    /////////////end of inserted
    RCONbits.IPEN = 1;              /* Enable interrupt priority */
    IPR1bits.RCIP = 1;              /* Make receive interrupt high priority */
    INTCONbits.GIEH = 1;            /* Enable all high priority interrupts */
}

void OC_Bias(Bias_OP_t bias)
{
    switch (bias)
    {
        case BIAS1:
            n_BIAS_EN_1 = 1;
            Bias_1_Polarity = POSITIVE;
            Bias1_VM = V1000;
            break;
        case BIAS2:
            n_BIAS_EN_2 = 1;
            Bias_2_Polarity = POSITIVE;
            Bias2_VM = V1000;
            break;
        case BIAS3:
            n_BIAS_EN_3 = 1;
            Bias_3_Polarity = POSITIVE;
            Bias3_VM = V1000;
            break;
        default:
            break;
    }
}

void GND_Bias(Bias_OP_t bias)
{
    switch (bias)
    {
        case BIAS1:
            n_BIAS_EN_1 = 1;
            SEL_HV_1 = 0;
            SEL_OP_A0_1 = 0;
            SEL_OP_A1_1 = 0;
            n_BIAS_EN_1 = 0;
            Bias_1_Polarity = POSITIVE;
            break;
        case BIAS2:
            n_BIAS_EN_2 = 1;
            SEL_HV_2 = 0;
            SEL_OP_A0_2 = 0;
            SEL_OP_A1_2 = 0;
            n_BIAS_EN_2 = 0;
            Bias_2_Polarity = POSITIVE;
            break;
        case BIAS3:
            n_BIAS_EN_3 = 1;
            SEL_HV_3 = 0;
            SEL_OP_A0_3 = 0;
            SEL_OP_A1_3 = 0;
            n_BIAS_EN_3 = 0;
            Bias_3_Polarity = POSITIVE;
            break;
        default:
            break;
    }
}

void EXTV1_Bias(Bias_OP_t bias)
{
    switch (bias)
    {
        case BIAS1:
            n_BIAS_EN_1 = 1;
            SEL_HV_1 = 0;
            SEL_OP_A0_1 = 0;
            SEL_OP_A1_1 = 1;
            n_BIAS_EN_1 = 0;
            Bias_1_Polarity = POSITIVE;
            Bias1_VM = V1000;
            break;
        case BIAS2:
            n_BIAS_EN_2 = 1;
            SEL_HV_2 = 0;
            SEL_OP_A0_2 = 0;
            SEL_OP_A1_2 = 1;
            n_BIAS_EN_2 = 0;
            Bias_2_Polarity = POSITIVE;
            Bias2_VM = V1000;
            break;
        case BIAS3:
            n_BIAS_EN_3 = 1;
            SEL_HV_3 = 0;
            SEL_OP_A0_3 = 0;
            SEL_OP_A1_3 = 1;
            n_BIAS_EN_3 = 0;
            Bias_3_Polarity = POSITIVE;
            Bias3_VM = V1000;
            break;
        default:
            break;
    }
}

void EXTV2_Bias(Bias_OP_t bias)
{
    switch (bias)
    {
        case BIAS1:
            n_BIAS_EN_1 = 1;
            SEL_HV_1 = 0;
            SEL_OP_A0_1 = 1;
            SEL_OP_A1_1 = 1;
            n_BIAS_EN_1 = 0;
            Bias_1_Polarity = POSITIVE;
            Bias1_VM = V1000;
            break;
        case BIAS2:
            n_BIAS_EN_2 = 1;
            SEL_HV_2 = 0;
            SEL_OP_A0_2 = 1;
            SEL_OP_A1_2 = 1;
            n_BIAS_EN_2 = 0;
            Bias_2_Polarity = POSITIVE;
            Bias2_VM = V1000;
            break;
        case BIAS3:
            n_BIAS_EN_3 = 1;
            SEL_HV_3 = 0;
            SEL_OP_A0_3 = 1;
            SEL_OP_A1_3 = 1;
            n_BIAS_EN_3 = 0;
            Bias_3_Polarity = POSITIVE;
            Bias3_VM = V1000;
            break;
        default:
            break;
    }
}

void INT_LV_Bias(Bias_OP_t bias)
{
    switch (bias)
    {
        case BIAS1:
            n_BIAS_EN_1 = 1;
            SEL_OP_A0_1 = 1;
            SEL_OP_A1_1 = 0;
            n_BIAS_EN_1 = 0;
            SEL_HV_1 = 0;
            break;
        case BIAS2:
            n_BIAS_EN_2 = 1;
            SEL_OP_A0_2 = 1;
            SEL_OP_A1_2 = 0;
            n_BIAS_EN_2 = 0;
            SEL_HV_2 = 0;
            break;
        case BIAS3:
            n_BIAS_EN_3 = 1;
            SEL_OP_A0_3 = 1;
            SEL_OP_A1_3 = 0;
            n_BIAS_EN_3 = 0;
            SEL_HV_3 = 0;
            break;
        default:
            break;
    }
}

void INT_HV_Bias(Bias_OP_t bias)
{
    switch (bias)
    {
        case BIAS1:
            n_BIAS_EN_1 = 1;
            SEL_OP_A0_1 = 1;
            SEL_OP_A1_1 = 0;
            n_BIAS_EN_1 = 0;
            SEL_HV_1 = 1;
            break;
        case BIAS2:
            n_BIAS_EN_2 = 1;
            SEL_OP_A0_2 = 1;
            SEL_OP_A1_2 = 0;
            n_BIAS_EN_2 = 0;
            SEL_HV_2 = 1;
            break;
        case BIAS3:
            n_BIAS_EN_3 = 1;
            SEL_OP_A0_3 = 1;
            SEL_OP_A1_3 = 0;
            n_BIAS_EN_3 = 0;
            SEL_HV_3 = 1;
            break;
        default:
            break;
    }
}

static void Set_Voltage (uint16_t RDAC)
{
    uint8_t temp;

    temp = SSP1BUF;
    SSP1BUF = 0x04 | ((uint8_t)(RDAC >> 8));     /* write serial & RDAC MSB */
    while (SSP1STATbits.BF == 0) {};
    temp = SSP1BUF;
    SSP1BUF  = (uint8_t)(RDAC & 0x00FF);      /* 1% performance, write enable */
    while (SSP1STATbits.BF == 0) {};

}

void Bias1_Voltage(Bias_V_t voltage)
{
    uint16_t RDAC;

    Bias1_VM = voltage;

    switch (voltage)
    {
        case V3V3:
            RDAC = 0x0152;
            CS_LV_1 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_1 = 1;
            Bias_1_Polarity = POSITIVE;
            break;
        case V5:
            RDAC = 0x01FF;
            CS_LV_1 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_1 = 1;
            Bias_1_Polarity = POSITIVE;
            break;
        case V10:
            RDAC = 0x03FF;
            CS_LV_1 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_1 = 1;
            Bias_1_Polarity = POSITIVE;
            break;

        case V50_NEG:
            SEL_100V_1 = 0;
            SEL_NEG_1 = 1;
            Bias_1_Polarity = NEGATIVE;
            break;
        case V50_POS:
            SEL_100V_1 = 0;
            SEL_NEG_1 = 0;
            Bias_1_Polarity = POSITIVE;
            break;
        case V100_NEG:
            SEL_100V_1 = 1;
            SEL_NEG_1 = 1;
            Bias_1_Polarity = NEGATIVE;
            break;
        case V100_POS:
            SEL_100V_1 = 1;
            SEL_NEG_1 = 0;
            Bias_1_Polarity = POSITIVE;
            break;
        default:
            break;
    }
}

void Bias2_Voltage(Bias_V_t voltage)
{
    uint16_t RDAC;

    Bias2_VM = voltage;

    switch (voltage)
    {
        case V3V3:
            RDAC = 0x0152;
            CS_LV_2 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_2 = 1;
            Bias_2_Polarity = POSITIVE;
            break;
        case V5:
            RDAC = 0x01FF;
            CS_LV_2 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_2 = 1;
            Bias_2_Polarity = POSITIVE;
            break;
        case V10:
            RDAC = 0x03FF;
            CS_LV_2 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_2 = 1;
            Bias_2_Polarity = POSITIVE;
            break;
        case V50_NEG:
            SEL_100V_2 = 0;
            SEL_NEG_2 = 1;
            Bias_2_Polarity = NEGATIVE;
            break;
        case V50_POS:
            SEL_100V_2 = 0;
            SEL_NEG_2 = 0;
            Bias_2_Polarity = POSITIVE;
            break;
        case V100_NEG:
            SEL_100V_2 = 1;
            SEL_NEG_2 = 1;
            Bias_2_Polarity = NEGATIVE;
            break;
        case V100_POS:
            SEL_100V_2 = 1;
            SEL_NEG_2 = 0;
            Bias_2_Polarity = POSITIVE;
            break;
        default:
            break;
    }
}

void Bias3_Voltage(Bias_V_t voltage)
{
    uint16_t RDAC;

    Bias3_VM = voltage;

    switch (voltage)
    {
        case V3V3:
            RDAC = 0x0152;
            CS_LV_3 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_3 = 1;
            Bias_3_Polarity = POSITIVE;
            break;
        case V5:
            RDAC = 0x01FF;
            CS_LV_3 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_3 = 1;
            Bias_3_Polarity = POSITIVE;
            break;
        case V10:
            RDAC = 0x03FF;
            CS_LV_3 = 0; /* SYNC pulse */
            Set_Voltage (RDAC);
            CS_LV_3 = 1;
            Bias_3_Polarity = POSITIVE;
            break;
        case V50_NEG:
            SEL_100V_3 = 0;
            SEL_NEG_3 = 1;
            Bias_3_Polarity = NEGATIVE;
            break;
        case V50_POS:
            SEL_100V_3 = 0;
            SEL_NEG_3 = 0;
            Bias_3_Polarity = POSITIVE;
            break;
        case V100_NEG:
            SEL_100V_3 = 1;
            SEL_NEG_3 = 1;
            Bias_3_Polarity = NEGATIVE;
            break;
        case V100_POS:
            SEL_100V_3 = 1;
            SEL_NEG_3 = 0;
            Bias_3_Polarity = POSITIVE;
            break;
        default:
            break;
    }
}

void Select_Voltage_Monitor(Bias_OP_t bias)
{
    Bias_V_t voltage;

    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA5 = 0;
    TRISFbits.TRISF2 = 0;

    switch (bias) /* select the voltage monitor according to the channel */
    {
        case BIAS1:
            voltage = Bias1_VM;
            if ((voltage == V50_NEG) || (voltage == V100_NEG))
            {
                TRISAbits.TRISA0 = 1;
            }
            else
            {
                TRISAbits.TRISA1 = 1;
            }
            break;
        case BIAS2:
            voltage = Bias2_VM;
            if ((voltage == V50_NEG) || (voltage == V100_NEG))
            {
                TRISAbits.TRISA2 = 1;
            }
            else
            {
                TRISAbits.TRISA3 = 1;
            }
            break;
        case BIAS3:
            voltage = Bias3_VM;
             if ((voltage == V50_NEG) || (voltage == V100_NEG))
            {
                TRISAbits.TRISA5 = 1;
            }
            else
            {
                TRISFbits.TRISF2 = 1;
            }
            break;
        default:
            break;

    }

    switch (voltage) /* set the monitor for the selected channel */
    {
        case V3V3:
        case V5:
            n_SEL_VM_10V = 1;
            n_SEL_VM_50V = 1;
            n_SEL_VM_100V = 1;
            n_SEL_VM_1000V = 1;
            break;
        case V10:
            n_SEL_VM_10V = 0;
            n_SEL_VM_50V = 1;
            n_SEL_VM_100V = 1;
            n_SEL_VM_1000V = 1;
            break;
        case V50_NEG:
        case V50_POS:
            n_SEL_VM_10V = 1;
            n_SEL_VM_50V = 0;
            n_SEL_VM_100V = 1;
            n_SEL_VM_1000V = 1;
            break;
        case V100_NEG:
        case V100_POS:
            n_SEL_VM_10V = 1;
            n_SEL_VM_50V = 1;
            n_SEL_VM_100V = 0;
            n_SEL_VM_1000V = 1;
            break;
        case V1000:
        default:
            n_SEL_VM_10V = 1;
            n_SEL_VM_50V = 1;
            n_SEL_VM_100V = 1;
            n_SEL_VM_1000V = 0;
            break;
    }
}

/* EOF */

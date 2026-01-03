/**********************************************************************************************************************
* Triteq Ltd. The Innovation Centre, Station Road, Hungerford, Berkshire. RG17 0DY
* Tel: +44 (0)1488 684554     Fax: +44 (0)1488 685335    www.triteq.com
* Copyright 2015 Triteq Ltd. All Rights Reserved.
***********************************************************************************************************************
*
*       Project:    MM20        Name:   Bias PSU
*
***********************************************************************************************************************/
/*!     @file       user.h
*       @brief      Header file for user.c
***********************************************************************************************************************
<pre>   @version
        Version     Date            Author      Description
        1.0.0       22/04/2015      Guy Mundy   Initial Release
</pre>
***********************************************************************************************************************/

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define BIAS_VM1_N      PORTAbits.RA0
#define BIAS_VM1_P      PORTAbits.RA1
#define BIAS_VM2_N      PORTAbits.RA2
#define BIAS_VM2_P      PORTAbits.RA3
#define BIAS_VM3_N      PORTAbits.RA5

#define SEL_100V_1      PORTBbits.RB0
#define SEL_NEG_1       PORTBbits.RB1
#define SEL_HV_1        PORTBbits.RB2
#define SEL_OP_A0_1     PORTBbits.RB3
#define SEL_OP_A1_1     PORTBbits.RB4
#define n_BIAS_EN_1     PORTBbits.RB5

#define n_SEL_VM_10V    PORTCbits.RC0
#define n_SEL_VM_50V    PORTCbits.RC1
#define n_SEL_VM_100V   PORTCbits.RC2

#define SEL_100V_2      PORTDbits.RD0
#define SEL_NEG_2       PORTDbits.RD1
#define SEL_HV_2        PORTDbits.RD2
#define SEL_OP_A0_2     PORTDbits.RD3
#define SEL_OP_A1_2     PORTDbits.RD4
#define n_BIAS_EN_2     PORTDbits.RD5

#define SEL_100V_3      PORTEbits.RE0
#define SEL_NEG_3       PORTEbits.RE1
#define SEL_HV_3        PORTEbits.RE2
#define SEL_OP_A0_3     PORTEbits.RE3
#define SEL_OP_A1_3     PORTEbits.RE4
#define n_BIAS_EN_3     PORTEbits.RE5
#define EN_485          PORTEbits.RE6
#define n_SEL_VM_1000V  PORTEbits.RE7

#define BIAS_VM3_P      PORTFbits.RF2
#define CS_LV_1         PORTFbits.RF5
#define CS_LV_2         PORTFbits.RF6
#define CS_LV_3         PORTFbits.RF7

#define BIAS_VM1_N_CH   (0x00<<2)
#define BIAS_VM1_P_CH   (0x01<<2)
#define BIAS_VM2_N_CH   (0x02<<2)
#define BIAS_VM2_P_CH   (0x03<<2)
#define BIAS_VM3_N_CH   (0x04<<2)
#define BIAS_VM3_P_CH   (0x07<<2)

typedef enum
{
    BIAS1 = 0x00,
    BIAS2,
    BIAS3
} Bias_OP_t;

typedef enum
{
    V3V3 = 0x00,
    V5,
    V10,
    V50_NEG,
    V50_POS,
    V100_NEG,
    V100_POS,
    V1000
} Bias_V_t;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

void Init_ADC(void);
void Init_Ports(void);
void Init_Timer(void);
void Init_SPI_1(void);
void Init_USART_1(void);
void OC_Bias(Bias_OP_t bias);
void GND_Bias(Bias_OP_t bias);
void EXTV1_Bias(Bias_OP_t bias);
void EXTV2_Bias(Bias_OP_t bias);
void INT_LV_Bias(Bias_OP_t bias);
void INT_HV_Bias(Bias_OP_t bias);
void Bias1_Voltage(Bias_V_t voltage);
void Bias2_Voltage(Bias_V_t voltage);
void Bias3_Voltage(Bias_V_t voltage);
void Select_Voltage_Monitor(Bias_OP_t bias);


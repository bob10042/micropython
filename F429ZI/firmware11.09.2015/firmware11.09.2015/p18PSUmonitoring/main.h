/**********************************************************************************************************************
* Triteq Ltd. The Innovation Centre, Station Road, Hungerford, Berkshire. RG17 0DY
* Tel: +44 (0)1488 684554     Fax: +44 (0)1488 685335    www.triteq.com
* Copyright 2015 Triteq Ltd. All Rights Reserved.
***********************************************************************************************************************
*
*       Project:    MM20        Name:   Bias PSU
*
***********************************************************************************************************************/
/*!     @file       main.h
*       @brief      Header file for main.c
***********************************************************************************************************************
<pre>   @version
        Version     Date            Author      Description
        1.0.0       22/04/2015      Guy Mundy   Initial Release
</pre>
***********************************************************************************************************************/

#define boardNO     17
#define nMessLength 12
#define POSITIVE    1
#define NEGATIVE    0

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

extern unsigned char fMess;
extern unsigned char mess2p24[];
extern unsigned char received[];
extern unsigned char myAdd;
extern unsigned char Bias_1_Polarity, Bias_2_Polarity, Bias_3_Polarity;



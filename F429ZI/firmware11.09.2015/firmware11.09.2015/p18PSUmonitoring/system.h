/**********************************************************************************************************************
* Triteq Ltd. The Innovation Centre, Station Road, Hungerford, Berkshire. RG17 0DY
* Tel: +44 (0)1488 684554     Fax: +44 (0)1488 685335    www.triteq.com
* Copyright 2015 Triteq Ltd. All Rights Reserved.
***********************************************************************************************************************
*
*       Project:    MM20        Name:   Bias PSU
*
***********************************************************************************************************************/
/*!     @file       System.h
*       @brief      header file for System.c
***********************************************************************************************************************
<pre>   @version
        Version     Date            Author      Description
        1.0.0       22/04/2015      Guy Mundy   Initial Release
</pre>
***********************************************************************************************************************/

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        8000000L
#define FCY             SYS_FREQ/4

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */

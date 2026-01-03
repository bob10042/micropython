/*         main.c   p18multiBd    */
// instruments uses RA3 while demo uses RB3 !!! the same for measurement boards
// bd address is not used as it is defined in EPROM-firmware is anyway unique
//      when used with demo pic 18
//          #define LEDPin      LATBbits.LATB3
//          #define LEDTris     TRISBbits.TRISB3
/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include <spi.h>
#include <timers.h>
#include <delays.h>
#include "typedefs.h"                       

// PIC18F4550 Configuration Bit Settings
#include <p18F4550.h>

// CONFIG1L
#pragma config PLLDIV = 5       // PLL Prescaler Selection bits (Divide by 5 (20 MHz oscillator input))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 2       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes from the 96 MHz PLL divided by 2)

// CONFIG1H
#pragma config FOSC = INTOSCIO_EC // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
//#pragma config FOSC = HS        // previous is much slower, next is twice as fast
//#pragma config FOSC = HSPLL_HS  // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting)
#pragma config VREGEN = ON      // USB Voltage Regulator Enable bit (USB voltage regulator enabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

/** V E C T O R  R E M A P P I N G *******************************************/
//#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
// change name of LEDPin and LEDTris
#define LEDPin      LATAbits.LATA3   //PORT a/b Pin 3 select 485
#define LEDTris     TRISAbits.TRISA3 //TRIS a/b Pin 3 as output
//#define LEDPin        LATBbits.LATB3
//#define LEDTris       TRISBbits.TRISB3

#define boardNO     16
#define nMessLenght 12

/** V A R I A B L E S ********************************************************/
#pragma static udata
unsigned char fMess = FALSE;
unsigned char mess2p24[nMessLenght];
unsigned char received[64];
unsigned char myAdd;
static char ver[] = "T10101";

/** P R I V A T E  P R O T O T Y P E S ***************************************/
//void _reset (void){
//    _asm goto _startup _endasm
//}
//#pragma idata

#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code
void UserInit(void) {
    ADCON0 = 0;
    ADCON1 = 0x0c;                      //Vss Vdd AN0, AN1, AN2 = A A A
    ADCON2 = 0xb8;                      //right justified; 20 Tad, Fosc/2
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    ADCON0bits.ADON = 1;
    //Input/output pins
    PIE1bits.SSPIE = 0;			//Turn off peripheral interrupt
    PIR1bits.SSPIF = 0;			//Clear any pending interrupt

    LEDTris = 0;
    LEDPin = 0;

//    myAdd = add0Slot+2*add1Slot+4*add2Slot+8*add3Slot+16*add4Slot;
    myAdd = boardNO;

    TMR0H = 0;				// clear timer0
    TMR0L = 0;				// clear timer0
    T0CONbits.PSA = 0;			// Assign prescaler to Timer 0
    T0CONbits.T0PS2 = 1;		// Setup prescaler
    T0CONbits.T0PS1 = 1;		// Will time out every 51 us based on
    T0CONbits.T0PS0 = 1;		// 20 MHz Fosc
    T0CONbits.T0CS = 0;			// Increment on instuction cycle

    OpenTimer0 (TIMER_INT_OFF & T0_SOURCE_INT & T0_8BIT & T0_PS_1_1);
    //Enable interrupts
    INTCONbits.PEIE = 0;                // Turn on peripheral interrupts
    INTCONbits.GIE = 0;			// Turn on global interrupts
}   //end UserInit

/******************************************************************************
 * Function:        void Read3551(char *data)
 * Input:           Pointer to a string; must be three bytes min
 *****************************************************************************/

void send12Data(unsigned char res[]) {  // send result to pic24
    int nn, j;
    
    LEDPin = 1;
    for (nn=0;nn<20;nn++);
    for (j=0;j<12;j++){
        WriteUSART(res[j]);        //send
        for (nn=0;nn<20;nn++);
    }
    for (nn=0;nn<20;nn++);
    LEDPin = 0;
    for (nn=0;nn<200;nn++);
}
/** D E C L A R A T I O N S **************************************************/
#pragma code
/******************************************************************************
 * Function:        void main(void)
 *****************************************************************************/
void main(void){
    unsigned int i, j;
    unsigned char * pch;

//    ADCON1 |= 0x0F;

    UserInit();                     // See user.c & .h
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
    OpenUSART (USART_TX_INT_OFF &   // Open the USART: 8N1,19200 baud
             USART_RX_INT_ON &
             USART_ASYNCH_MODE &
             USART_EIGHT_BIT &
             USART_CONT_RX &
             USART_BRGH_HIGH,25);
    // next instruction is NEEDED to speed up 4 times
    BAUDCON = 0x08;                 //12=19200; 25 = 9800
    RCONbits.IPEN = 1;              /* Enable interrupt priority */
    IPR1bits.RCIP = 1;              /* Make receive interrupt high priority */
    INTCONbits.GIEH = 1;            /* Enable all high priority interrupts */
    while(1) {
        if (fMess) {
            fMess = FALSE;
            if (received[4] == 127) {
                mess2p24[4] = ver[0];   //version request
                mess2p24[5] = ver[1];
                mess2p24[6] = ver[2];
                mess2p24[7] = ver[3];
                mess2p24[8] = ver[4];
                mess2p24[9] = ver[5];
            }
            else {
                ADCON0 &= 0xf3;             // ch 0 temperature
                ADCON0 |= 0x03;
                while (ADCON0bits.GO == 1);
                mess2p24[4]= ADRESH;
                mess2p24[5] = ADRESL;

                for (j=0;j<100;j++);

                ADCON0 &= 0xfb;             // ch 1
                ADCON0 |= 0x04;
                ADCON0 |= 0x03;
                while (ADCON0bits.GO == 1);
                mess2p24[6] = ADRESH;
                mess2p24[7] = ADRESL;

                for (j=0;j<100;j++);
            }
            send12Data(mess2p24);
        }
    }                                       // end while
}                                           // end main

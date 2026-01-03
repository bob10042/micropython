/*   Measurement board main.c     */
//      when used with demo pic 18
//          myAdd needs to be hardcoded
//          #define LEDPin      LATAbits.LATB3
//          #define LEDTris     TRISAbits.TRISB3
//          instruction around 185 commented out:
//             myAdd = add0Slot+2*add1Slot+4*add2Slot+8*add3Slot+16*add4Slot;
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
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
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
// for AutoCAF
#define LEDPin      LATAbits.LATA3   //Define LEDPin as PORT a Pin 3
#define LEDTris     TRISAbits.TRISA3 //Define LEDTris as TRISA Pin 3
// for demo pic18
//#define LEDPin      LATBbits.LATB3   //Define LEDPin as PORT b Pin 3
//#define LEDTris     TRISBbits.TRISB3 //Define LEDTris as TRISB Pin 3

#define CS_PTBoard  LATBbits.LATB2   //Define CS_PTBoard as TRISB Pin 2
#define tris_CS     TRISBbits.TRISB2 //Define tris_CS as TRISB Pin 2
#define SCKpin      LATBbits.LATB1   //Define CS_PTBoard as TRISB Pin 1
#define SCKTris     TRISBbits.TRISB1 //Define tris_CS as TRISB Pin 1
#define SDIpin      LATBbits.LATB0   //Define CS_PTBoard as TRISB Pin 0
#define SDITris     TRISBbits.TRISB0 //Define tris_CS as TRISB Pin 0
#define add0        LATBbits.LATB4   //channel addresses
#define add0Tris    TRISBbits.TRISB4
#define add1        LATAbits.LATA5
#define add1Tris    TRISAbits.TRISA5
#define add2        LATAbits.LATA4
#define add2Tris    TRISAbits.TRISA4
#define mux1Pin     LATBbits.LATB5
#define mux1Tris    TRISBbits.TRISB5
//#define mux2Pin     PORTEbits.RE0     // both work!
#define mux2Pin     LATEbits.LATE0
#define mux2Tris    TRISEbits.TRISE0

#define add0Slot    PORTEbits.RE1   // board addresses
#define add0STris   TRISEbits.TRISE1
#define add1Slot    PORTEbits.RE2
#define add1STris   TRISEbits.TRISE2
#define add2Slot    PORTCbits.RC0
#define add2STris   TRISCbits.TRISC0
#define add3Slot    PORTCbits.RC1
#define add3STris   TRISCbits.TRISC1
#define add4Slot    PORTCbits.RC2
#define add4STris   TRISCbits.TRISC2

#define boardNO     1
#define nMessLenght 12
#define settlingTime 400 // 140

/** V A R I A B L E S ********************************************************/
#pragma static udata
BOOL gSampleFlag;
BOOL AcquireData;
unsigned char fMmess = FALSE;
unsigned char fSmess = FALSE;
unsigned char mess2p24[nMessLenght];
unsigned char received[60]; // needed to store message od other MB to p24
unsigned char data48[48];
unsigned char mymessage[34];
unsigned int  TimerCounterDJ;
unsigned char myAdd;
static char ver[] = "M10102";
/** P R I V A T E  P R O T O T Y P E S ***************************************/
//void _reset (void){
//    _asm goto _startup _endasm
//}
//#pragma idata

#pragma code
void Read3551(unsigned int);
unsigned char ReadSPIdj( void );
void getsSPI(unsigned char *, unsigned char);
void ProcessIO(void);
void AD22(unsigned int);
void setAdd(unsigned int);
unsigned int pow2dj(unsigned int);
unsigned char getChannel(unsigned int, unsigned int);
void send3Data(unsigned char *, unsigned char);
/** D E C L A R A T I O N S **************************************************/
#pragma code
void UserInit(void) {
//    nRec = 0;
    gSampleFlag = FALSE;
    TimerCounterDJ = settlingTime;
    //Input/output pins
    PIE1bits.SSPIE = 0;			//Turn off peripheral interrupt
    PIR1bits.SSPIF = 0;			//Clear any pending interrupt
    OpenSPI(SPI_FOSC_16, MODE_11, SMPMID);//master
    TRISBbits.TRISB0 = 1;               //SDI input 
    TRISBbits.TRISB1 = 0;               //SCK output
    TRISBbits.TRISB2 = 0; 		//CS
    LEDTris = 0;
    LEDPin = 0;
    add0=0;
    add0Tris = 0;
    add1 = 0;
    add1Tris = 0;
    add2 = 0;
    add2Tris = 0;
    mux1Pin = 0;
    mux1Tris = 0;
    mux2Pin = 0;
    mux2Tris = 0;

    add0STris = 1;
    add1STris = 1;
    add2STris = 1;
    add3STris = 1;
    add4STris = 1;

    myAdd = add0Slot+2*add1Slot+4*add2Slot+8*add3Slot+16*add4Slot;
//    myAdd = boardNO;

    PORTBbits.RB2 = 1;                  // CS_PTBoard = 0;
//    PIE1bits.SSPIE = 1;		// do not Turn on SPI interrupt

    TMR0H = 0;				// clear timer0
    TMR0L = 0;				// clear timer0
    T0CONbits.PSA = 0;			// Assign prescaler to Timer 0
    T0CONbits.T0PS2 = 1;		// Setup prescaler
    T0CONbits.T0PS1 = 1;		// Will time out every 51 us based on
    T0CONbits.T0PS0 = 1;		// 20 MHz Fosc
    T0CONbits.T0CS = 0;			// Increment on instruction cycle

    OpenTimer0 (TIMER_INT_OFF & T0_SOURCE_INT & T0_8BIT & T0_PS_1_1);
    //Enable interrupts
    IPR1bits.RCIP = 1;                  // test !!!
    INTCONbits.PEIE = 0;                // Turn on peripheral interrupts
    INTCONbits.GIE = 0;			// Turn on global interrupts
    //T0CONbits.TMR0ON = 1;
}   //end UserInit

/******************************************************************************
 * Function:        void Read3551(char *data)
 * Input:           Pointer to a string; must be three bytes min
 *****************************************************************************/
unsigned char ReadSPIdj( void ){
  unsigned char TempVar;
  TempVar = SSPBUF;             // Clear BF
  PIR1bits.SSPIF = 0;           // Clear interrupt flag
  SSPBUF = 0x00;                // initiate bus cycle
  //while ( !SSPSTATbits.BF );  // wait until cycle complete
  while(!PIR1bits.SSPIF);       // wait until cycle complete
  return ( SSPBUF );            // return with byte read
}

void Read3551(unsigned int i){      //AD to DATA48[3i+2],DATA48[3i+1],DATA48[3i]
  i *= 3;
  data48[i]   = ReadSPIdj();
  data48[i+1] = ReadSPIdj();
  data48[i+2] = ReadSPIdj();
}

void AD22(unsigned int locAdd) {    // perform AD conversion
    unsigned int nn;

    AcquireData = TRUE;
    while (AcquireData){
        if (INTCONbits.TMR0IF){
            TimerCounterDJ--;
            if (TimerCounterDJ==0){     // if rolled over, set flag
                TimerCounterDJ = settlingTime;     // 1000 = 1 sec
                gSampleFlag = 1;
                  //  WriteTimer0(250); // 1 msec for timer
            }
            INTCONbits.TMR0IF = 0;
        }
        if (gSampleFlag) {
            PIE1bits.RCIE = 0;          // test !!!!!!!!!!!!
            PORTBbits.RB2 = 0;          // CS_PTBoard = 0;
            for (nn=0;nn<5;nn++);
            if (PORTBbits.RB0 == (unsigned)0) {
                Read3551(locAdd);
                AcquireData = FALSE;
            }
            PORTBbits.RB2 = 1;          // CS_PTBoard = 1;
            PIE1bits.RCIE = 1;

            gSampleFlag = 0;
        }
    }
}

void setAdd(unsigned int nCH){          //channel select
    if (nCH & 1)                        // add line 0
        add0 = 1;
    else
        add0 = 0;
    if (nCH & 2)                        // add line 1
        add1 = 1;
    else
        add1 = 0;
    if (nCH & 4)                        // add line 2
        add2 = 1;
    else
        add2 = 0;
    if (nCH<8){
        mux1Pin = 1;                    // enable mux1/2
        mux2Pin = 0;
    }
    else {
        mux1Pin = 0;
        mux2Pin = 1;
    }
}
unsigned int pow2dj(unsigned int nCH) { // 2 ** nCH
    unsigned int j, k = 2;
    if (nCH < 1) k = 1;
    else
        for (j=1; j<nCH; j++)
            k *= 2;
    return k;
}
unsigned char getChannel(unsigned int nCH, unsigned int maxNSamples) {
    unsigned char succ = 1;

    setAdd(nCH);
    AD22(nCH);          // implemen: untill stable or maxNSamples reached HERE!!
    return succ;
}

void send54Data(unsigned char res[]) {  // send result to pic24
    int nn, j;
    unsigned char mess2p24[nMessLenght];
    
    LEDPin = 1;
    mess2p24[0] = 2;                    // stx
    mess2p24[1] = 54;                    // pic24 add (not used)
    mess2p24[2] = 100;                    // not used
    mess2p24[3] = myAdd;                // add of board that is sending result
                                        // 48 data sent on its own
    mess2p24[8] = 0;                    // cs
    mess2p24[9] = 3;                    // etx
    for (nn=0;nn<20;nn++);
    for (j=0;j<4;j++){
        WriteUSART(mess2p24[j]);        //send first 4
        for (nn=0;nn<20;nn++);
    }
    for (j=0;j<48;j++) {
        WriteUSART(res[j]);             // send data
        for (nn=0;nn<20;nn++);
        }
    for (j=8;j<10;j++) {                // send last 2
        WriteUSART(mess2p24[j]);
        for (nn=0;nn<20;nn++);
    }
    for (nn=0;nn<20;nn++);
    LEDPin = 0;
    for (nn=0;nn<200;nn++);
}
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
    unsigned int i, j, flagG, nCh, mmaxCh, flagLastCh=0;
    long int maxL, tempL;
    long int t1, t2,t3;
    ADCON1 |= 0x0F;

    UserInit();                     // See user.c & .h
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
        if (fSmess) {               // send message
            fSmess = FALSE;
            if (received[4] == 127) {
                mess2p24[0]=2;
                mess2p24[1]=12;
                mess2p24[2]=100;
                mess2p24[3]=myAdd;
                mess2p24[4] = ver[0];   //version request
                mess2p24[5] = ver[1];
                mess2p24[6] = ver[2];
                mess2p24[7] = ver[3];
                mess2p24[8] = ver[4];
                mess2p24[9] = ver[5];
                mess2p24[10]=0;
                mess2p24[11]=3;
                send12Data(mess2p24);   //  send data
            }
            else
                send54Data(data48);
        }
        if (fMmess) {               // if type tMeasure
            fMmess = FALSE;
            if (mymessage[32] != 0) flagLastCh = 1;
            else flagLastCh = 0;
            for (j=0;j<48;j++) data48[j] = 0xff;
            for (nCh=0;nCh<16;nCh++) {
                if (nCh<8) {
                    if (mymessage[2*myAdd] & pow2dj(nCh))
                        getChannel(nCh, 0);
                }
                else {
                    if (mymessage[2*myAdd+1] & pow2dj(nCh-8))
                        getChannel(nCh, 0);
                }
            }
            if (flagLastCh) {
                mmaxCh = 0;
                maxL = (long)0;
                tempL = (long)0;
                if(mymessage[2*myAdd] & pow2dj(0)) {
                    t1 = (long)data48[2];
                    t2 = (long)256*(long)data48[1];
                    t3 = (long)256*(long)256*(long)data48[0];
                    maxL = t1 +t2 +t3;
                }
                // next one doesn't work !!!!!!!
                //                    maxL = (long)256*(long)256*(long)data48[0]+(long)256*(long)data48[1]+(long)data48[2];
                for (j=1;j<16;j++) {                       // find max current
                    i = j;
                    if (j < 8) {
                        if(mymessage[2*myAdd] & pow2dj(j)) {
                            // next one doesn't work !!!!!!!
                            //   tempL = (long)256*(long)256*(long)data48[j*3]+(long)256*(long)data48[j*3+1]+(long)data48[j*3+2];
                            t1 = (long)data48[j*3+2];
                            t2 = (long)256*(long)data48[j*3+1];
                            t3 = (long)256*(long)256*(long)data48[j*3];
                            tempL = t1 + t2 + t3;
                            if (tempL > maxL) {
                                mmaxCh = i;
                                maxL = tempL;
                            }
                        }
                    }
                    else {
                        if(mymessage[2*myAdd+1] & pow2dj(j-8)) {
                                // next one doesn't work !!!!!!!
                            //                tempL = (long)256*(long)256*(long)data48[j*3]+(long)256*(long)data48[j*3+1]+(long)data48[j*3+2];
                            t1 = (long)data48[j*3+2];
                            t2 = (long)256*(long)data48[j*3+1];
                            t3 = (long)256*(long)256*(long)data48[j*3];
                            tempL = t1 + t2 + t3;
                            if (tempL > maxL) {
                                mmaxCh = i;
                                maxL = tempL;
                            }
                        }
                    }
                }
                setAdd(mmaxCh);
            }
        }
    }               //end while
}                   //end main

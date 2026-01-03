/**** PIC24FJ128GA010 ***********/
#include "p24fxxxx.h"
#include <p24FJ128GA010.h>
#include "uart2.h"

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_XT & FNOSC_PRI)

#define LEDPin       LATBbits.LATB4     //Define LEDPin as PORT b Pin 1
#define LEDTris      TRISBbits.TRISB4   //Define LEDTris as TRISB Pin 1
#define CTRLPin      LATBbits.LATB14    //Define LEDPin as PORT b Pin 1
#define CTRLTris     TRISBbits.TRISB14  //Define LEDTris as TRISB Pin 1
#define TRUE            1
#define FALSE           0
#define LEDgreen      LATGbits.LATG9
#define LEDgreenTris  TRISGbits.TRISG9
#define LEDred        LATGbits.LATG6
#define LEDredTris    TRISGbits.TRISG6
#define LEDyellow     LATGbits.LATG7
#define LEDyellowTris TRISGbits.TRISG7

#define en_dj         LATDbits.LATD4
#define en_Tris       TRISDbits.TRISD4
#define rw_dj         LATDbits.LATD5
#define rw_Tris       TRISDbits.TRISD5
#define rs_dj         LATBbits.LATB15
#define rs_Tris       TRISBbits.TRISB15

#define nMessLenght     12                      // R request to pic18

void LCDInit(void);
void LCDHome(void);
void LCDL1Home(void);
void LCDL2Home(void);
void LCDClear(void);
void LCDPut(char);
void sendData(unsigned char *, unsigned int);
void sendData2PC(unsigned char *, unsigned int);

//unsigned char S3Flag, S4Flag, S5Flag, S6Flag;
unsigned char received1[64];
unsigned char received2[40];
unsigned int  nRec1, nRec2, expected1, expected2;
unsigned char fMess, fMess2;
static char ver[] = "C10103";

void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    received2[nRec2] = U2RXREG;                 // Get char from PC
    if (nRec2) nRec2++;
    else
        if (received2[0] == 0x02) nRec2++;

    if (nRec2 == 2)
        expected2 = received2[1];

    if (nRec2 > 2)
        if (nRec2 == expected2 && received2[nRec2-1] == 0x03) {
                fMess2 = 1;
                nRec2 = 0;
        }
    if (nRec2 > 2 && nRec2 >= expected2)
        nRec2 = 0;
    IFS1bits.U2RXIF = 0;
}
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
	IFS1bits.U2TXIF = 0;                    // not enabled
}

void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    received1[nRec1] = U1RXREG;                 // Get char from USART
//    LCDPut(received1[nRec1]+48);              // test only
    if (nRec1) nRec1++;
    else {
     //   LCDPut(received1[0]+48);              // test only
        if (received1[0] == 0x02) nRec1++;
    }
    if (nRec1 == 2)
        expected1 = received1[1];

    if (nRec1 == expected1 && received1[nRec1-1] == 0x03) {
        fMess = 1;
        nRec1 = 0;
    }
    if ((nRec1 > 2) && (nRec1 >= expected1))
        nRec1 = 0;
    IFS0bits.U1RXIF = 0;
}
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;                        // not enabled
}

void InitUART1() {
    U1BRG = 25;                                 // 25 for 9600; 12 for 19200

    U1MODEbits.UARTEN = 1;                      // UART1 is Enabled
    U1MODEbits.USIDL = 0;                       // Continue operation at Idlestate
    U1MODEbits.IREN = 0;                        // IrDA En/Decoder is disabled
    U1MODEbits.RTSMD = 0;                       // flow control mode
    U1MODEbits.UEN = 0b10;                      // UTX, RTX, U1CTS, U1RTS are enabled
    U1MODEbits.WAKE = 0;                        // Wake-up on start bit is enabled
    U1MODEbits.LPBACK = 0;                      // Loop-back is disabled
    U1MODEbits.ABAUD = 0;                       // auto baud is disabled
    U1MODEbits.RXINV = 0;                       // No RX inversion
    U1MODEbits.BRGH = 0;                        // low boud rate
    U1MODEbits.PDSEL = 0b00;                    // 8bit no parity
    U1MODEbits.STSEL = 0;                       // one stop bit

    U1STAbits.UTXISEL1 = 0b00;
    U1STA &= 0xDFFF;                            // clear TXINV by bit masking
    U1STAbits.UTXBRK = 0;                       // sync break tx is disabled
    U1STAbits.UTXEN = 1;                        // transmit  is enabled
    U1STAbits.URXISEL = 0b00;                   // interrupt flag bit
    U1STAbits.ADDEN = 0;                        // address detect mode is disabled

    IFS0bits.U1RXIF = 0;                        // clear interrupt flag of rx
    IEC0bits.U1RXIE = 1;                        // enable rx recieved data interrupt
}
void InitUART2(void) {
    // configure U2MODE
    U2MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
    U2MODEbits.USIDL = 0;	// Bit13 Continue in Idle
    U2MODEbits.IREN = 0;	// Bit12 No IR translation
    U2MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
    U2MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
    U2MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0;	// Bit6 No Loop Back
    U2MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
    U2MODEbits.RXINV = 0;	// Bit4 IdleState = 1
    U2MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
    U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit

    U2BRG = BAUDRATEREG2;	// baud rate

    // Load all values in for U1STA SFR
    U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
    U2STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
    U2STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
    U2STAbits.UTXBRK = 0;	//Bit11 Disabled
    U2STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
    U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
    U2STAbits.TRMT = 0;		//Bit8 *Read Only bit*
    U2STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
    U2STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
    U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
    U2STAbits.PERR = 0;		//Bit3 *Read Only Bit*
    U2STAbits.FERR = 0;		//Bit2 *Read Only Bit*
    U2STAbits.OERR = 0;		//Bit1 *Read Only Bit*
    U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

    IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
    IEC1bits.U2TXIE = 0;	// Enable Transmit Interrupts
    IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
    IEC1bits.U2RXIE = 1;	// Enable Recieve Interrupts

    U2MODEbits.UARTEN = 1;	// And turn the peripheral on

    U2STAbits.UTXEN = 1;
}

void InitPorts(void) {
    TRISD = 0x20C0;                             // D6,7,13 inputs
//    TRISA = 0x0080;                           // A7 is input, A6-A0 output
    TRISA = 0x0000;
    AD1PCFG = 0xFFFF;
    TRISE = 0xFF;
    LEDTris = 0;
    LEDPin = 0;
    CTRLTris = 0;
    CTRLPin = 0;
    LEDgreenTris = 0;                               // LEDs
    LEDgreen = 1;
    LEDredTris  = 0;
    LEDred = 1;
    LEDyellowTris = 0;
    LEDyellow = 1;
    en_Tris = 0;                                    // display control
    rw_Tris = 0;
    rs_Tris = 0;
}

void sendData(unsigned char message[], unsigned int nm) {   // send2p18
    unsigned int n, i;
    LEDPin = 1;
    for (n=0;n<100;n++);
    for (i=0; i<nm; i++) {
        U1TXREG = message[i];
        while(!U1STAbits.TRMT);
        for (n=0;n<400;n++);                                // 400
    }
    for (n=0;n<200;n++);                                    // 200
    LEDPin = 0;
}
void sendData2PC(unsigned char message[], unsigned int nm) {// send2PC
    unsigned int i,j;
    for (i=0;i<nm;i++) {
        for (j=0;j<5;j++);
        U2TXREG = message[i];
        while(!U2STAbits.TRMT);
    }
}

void djInitLCD(void) {                              // Init display
    unsigned int k;
    PMMODE = 0x3FF;
    PMCON = 0x8303;
    PMAEN = 0x0001;
    for (k=0;k<50000;k++);

    PMDIN1 = 0x38;
    for (k=0;k<50000;k++);
    PMDIN1 = 0x0c;
    for (k=0;k<50000;k++);
    PMDIN1 = 0x01;
    for (k=0;k<50000;k++);
    PMDIN1 = 0x06;                                  // clear display
    en_dj = 0;
    PMDIN1 = 0x80;                                  // dd ram
    for (k=0;k<1000;k++);

    LCDClear();
    LCDHome();
}

void djprint(char ch, int pp) {                     // print ch at nn NOT TESTED
    int k;
    for (k=0;k<1550;k++);
    PMDIN1 = (char)(pp);
    for (k=0;k<1550;k++);
    for (k=0;k<1550;k++);
    rw_dj = 1;
    for (k=0;k<50;k++);
    PMDIN1 = ch;
    for (k=0;k<50;k++);
    rw_dj = 0;
    for (k=0;k<50000;k++);
}

int main(void) {
    unsigned int i, ii, k;
    unsigned long jj;
    unsigned char mess2p18[nMessLenght], fSelBd[16];
    unsigned char nextBd, nWaitingFrom = 20;
    unsigned char fFirst, fNOK, fContinue;
    unsigned char text[79];
    RCONbits.SWDTEN = 0;// Disable Watch Dog Timer
//    LCDInit();

    djInitLCD();
    for (i=0;i<80;i++) text[i] = ' ';         // prepare spaces
        
    LCDHome();
    text[41]='G';
    text[42]='E';
    text[43]='N';
    text[44]='3';


    text[60] = 'w';
    text[61] = 'w';
    text[62] = 'w';
    text[63] = '.';
    text[64] = 'g';
    text[65] = 'e';
    text[66] = 'n';
    text[67] = '3';
    text[68] = 's';
    text[69] = 'y';
    text[70] = 's';
    text[71] = 't';
    text[72] = 'e';
    text[73] = 'm';
    text[74] = 's';
    text[75] = '.';
    text[76] = 'c';
    text[77] = 'o';
    text[78] = 'm';

    for (i=0;i<79;i++) LCDPut(text[i]);

    LCDHome();
    
    InitUART2();	// Initialize UART2
    InitUART1();
    InitPorts();	// LEDs outputs, Switches Inputs

    fMess2 = 0;
    fMess  = 0;
    nRec1 = 0;
    nRec2 = 0;
    mess2p18[0] = 2;        //stx
    mess2p18[1] = 12;       //length
    mess2p18[2] = 0;        //to
    mess2p18[3] = 100;      //from
    mess2p18[4] = 0;        //data
    mess2p18[5] = 0;       // 0 = version request; 1 = NOT version request
    mess2p18[6] = 0;        //data
    mess2p18[7] = 0;        //data
    mess2p18[8] = 0;        //data
    mess2p18[9] = 0;        //data
    mess2p18[10] = 0;       //cs
    mess2p18[11] = 3;       //etx

    for (i=0; i<16; i++)        fSelBd[i] = 0;
    while(1) {
        if (fMess) {                  	// message from MB or PPS or TH (UART1)
            fMess = 0;
//            LCDPut(received1[3]+48);
            if (received1[3] == 16 || received1[3] == 17) { // if th or pps
                sendData2PC(received1, received1[1]);       // forward
//                if (received1[3] == 17) LEDred = 1;         // swich off red
                if (received1[3] == 16) LEDyellow = 1;      // swich off yellow
            }
            if (received1[3] >= 0 && received1[3] < 16) {   // if MB
                if (received1[1] == 12)                     // if version request
                    sendData2PC(received1, received1[1]);   // forward
                else {
                    if (received1[3] >= 0 && received1[3]<16){  // if MB data
//                        LEDgreen = 1;                       // green off
                        fFirst = 1;                         // for repeat
                        fNOK = 1;
                        fContinue = 0;
                        while (fNOK) {
                            fMess2 = 0;                     // not needed
                            sendData2PC(received1,received1[1]);// forward data
                            jj = 1;
                            while (jj < 200000 && fMess2 == 0) {// ack/nak
                                jj++;
                            }
                            if (fMess2) {
                                fMess2 = 0;
                                if (received2[4] == 21) {   // NAK
                                    if (fFirst) fFirst = 0; // go back; repeat
                                    else fNOK = 0;
                                }
                                else {
                                    fNOK = 0;               // ACK
                                    fContinue = 1;          // all fine; next bd
                            }   }
                            else fNOK = 0;                  // no message, finish
                    }   }
                    if (fContinue) {
                        fSelBd[nWaitingFrom] = 0;           // reset nWaitingFrom
                        nextBd = 0;
                        for (k=nWaitingFrom+1;k<16;k++) {   // look for next
                            if (fSelBd[k]) {
                                nextBd = k;
                                break;
                        }   }
                        if (nextBd) {
    //                        LCDPut('S');
    //                        LCDPut(nextBd+48);
                            mess2p18[2] = nextBd;
                            for (i=0;i<100;i++);
                            sendData(mess2p18, nMessLenght);// enable send
                            for (i=0;i<100;i++);
                            nWaitingFrom = k;
                        }
                        else nWaitingFrom = 20;
        }   }  }  }
        if (fMess2) {                           // message from PC uart2
	    fMess2 = 0;                         
            if (received2[2] == 19) {           // message to p24
                if (received2[4] == 0) {        // if request for p24 version
                    received2[4] = ver[0];
                    received2[5] = ver[1];
                    received2[6] = ver[2];
                    received2[7] = ver[3];
                    received2[8] = ver[4];
                    received2[9] = ver[5];
                    sendData2PC(received2, received2[1]);   //send version to PC
                }
                else{
                    if (received2[4] == 1) {                // cycles done
                        text[30]=received2[5];
                        text[31]=received2[6];
                        text[32]=received2[7];
                        text[33]=received2[8];
                        for (i=0;i<74;i++) LCDPut(text[i]);
                    }
                    if (received2[4] == 2) {                // cycles total
                        text[21]='C';
                        text[22]='y';
                        text[23]='c';
                        text[24]='l';
                        text[25]='e';
                        text[26]='s';
                        text[27]=':';

                        text[30]=' ';
                        text[31]=' ';
                        text[32]=' ';
                        text[33]='0';
                        text[34]='/';

                        text[35]=received2[5];
                        text[36]=received2[6];
                        text[37]=received2[7];
                        text[38]=received2[8];
                        LCDHome();
                        for (i=0;i<39;i++) LCDPut(text[i]);
                    }
                    if (received2[4] == 3) {                // TH
                        text[70]=received2[5];
                        text[71]=received2[6];
                        text[72]=received2[7];
                        text[76]=received2[8];
                        text[77]=received2[9];
                        LCDHome();
                        for (i=0;i<78;i++) LCDPut(text[i]);
                    }
                    if (received2[4] == 4) {                // redLED
                        if (received2[5] == 1)
                            LEDred = 0;         // swich on red
                        else
                            LEDred = 1;         // swich off red
                    }
                    if (received2[4] == 6) {                // redLED
                        if (received2[5] == 1)
                            LEDgreen = 0;         // swich on red
                        else
                            LEDgreen = 1;         // swich off red
                    }

                    if (received2[4] == 5) {                // caf/sir/TH
                        text[51]='A';
                        text[52]='u';
                        text[53]='t';
                        text[54]='o';
                        text[58]='2';
                        if (received2[5] == 1) {
                            text[55]='C';
                            text[56]='A';
                            text[57]='F';
                        }
                        else {
                            text[55]='S';
                            text[56]='I';
                            text[57]='R';
                        }
                        for (i=0;i<58;i++) LCDPut(text[i]);
                        LCDHome();
                        if (received2[6] == 1) {
                            text[60]=' ';
                            text[61]='T';   // T/H
                            text[62]='/';
                            text[63]='H';
                            text[64]=':';
                            text[65]=' ';
                            text[66]=' ';
                            text[67]=' ';
                            text[68]=' ';
                            text[69]=' ';
                            text[70]=' ';
                            text[71]='-';
                            text[72]='-';
                            text[73]=223;
                            text[74]='C';
                            text[75]=' ';
                            text[76]='-';
                            text[77]='-';
                            text[78]='%';
                        }
                        else {
                            text[60] = ' ';
                            text[61] = ' ';
                            text[62] = ' ';
                            text[63] = ' ';
                            text[64] = ' ';
                            text[65] = ' ';
                            text[66] = ' ';
                            text[67] = ' ';
                            text[68] = ' ';
                            text[69] = ' ';
                            text[70] = ' ';
                            text[71] = ' ';
                            text[72] = ' ';
                            text[73] = ' ';
                            text[74] = ' ';
                            text[75] = ' ';
                            text[76] = ' ';
                            text[77] = ' ';
                            text[78] = ' ';
                        }
                        for (i=0;i<79;i++) LCDPut(text[i]);
                    }
                    LCDHome();
                }
            }
            else {
            //    if (received2[2] == 17) LEDred = 0;         // red on
                if (received2[2] == 16) LEDyellow = 0;      // yellow on
                sendData(received2, received2[1]);          // forward to boards
                if (received2[2] == 18) {                   // if all MBs
                   // LEDgreen = 0;
                    for (ii=0;ii<100;ii++);
                    for (i=0;i<16;i++) fSelBd[i] = 0;
                    nWaitingFrom = 20;
                    for (i=0;i<16;i++) {                    // set fSelBd
                        if (received2[4+i*2] | received2[4+i*2+1])
                            fSelBd[i] = 1;
                    }
                    nextBd = 0;
                    for (k=0;k<16;k++) {
                        if (fSelBd[k]) {
                             nextBd = k;
                             break;
                    }   }
                    mess2p18[2] = nextBd;        // k/i for settlingtimedj = 140
                    //LCDPut('S');               // 1/1000 faster
                    for (k=0; k<1; k++)          // 3300/1000 more reliable
                        for (i=0; i<1000; i++);
//                    LCDPut(nextBd+48);
                    sendData(mess2p18, nMessLenght);
                    nWaitingFrom = nextBd;
                }
                nRec2 = 0;
}   }   }   }

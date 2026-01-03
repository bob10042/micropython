/***LCD Driver for PIC24.****/

#include	<p24FJ128GA010.h>

// Looptime is 1 us per loop count

// Define a fast instruction execution time in terms of loop time
// typically > 43us
#define	LCD_F_INSTR		50

// Define a slow instruction execution time in terms of loop time
// typically > 1.53ms
#define	LCD_S_INSTR		1600

// Define the startup time for the LCD in terms of loop time
// typically > 40ms
#define	LCD_STARTUP		50000

unsigned int	_uLCDloops;
void LCDInit(void);
void Wait(unsigned int);
void LCDClear(void);
void LCDHome(void);
void LCDL1Home(void);
void LCDL2Home(void);
void LCDClear(void);
void LCDPut(char);

void LCDInit()
{
	PMMODE = 0x3FF;
//PMMODEbits.BUSY = 0;
	PMCON = 0x8303;
	PMAEN = 0x0001;

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_F_INSTR;
	PMDIN1 = 0b00111000;			// Set the default function
	Wait(_uLCDloops);

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_F_INSTR;
	PMDIN1 = 0b00001100;
	Wait(_uLCDloops);

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_S_INSTR;
	PMDIN1 = 0b00000001;			// Clear the display
	Wait(_uLCDloops);

	_uLCDloops = LCD_STARTUP;
	Wait(_uLCDloops);

	_uLCDloops = LCD_S_INSTR;
	PMDIN1 = 0b00000110;			// Set the entry mode
	Wait(_uLCDloops);

	LCDClear();
	LCDHome();
}

void LCDHome()
{
	_uLCDloops = LCD_S_INSTR;
	PMADDR = 0x0000;
	PMDIN1 = 0b00000010;
	while(_uLCDloops)
	_uLCDloops--;
}

void LCDL1Home()
{
	_uLCDloops = LCD_S_INSTR;
	PMADDR = 0x0000;
	PMDIN1 = 0b10000000;
	while(_uLCDloops)
	_uLCDloops--;
}

void LCDL2Home()
{
	_uLCDloops = LCD_S_INSTR;
	PMADDR = 0x0000;
	PMDIN1 = 0b11000000;
	while(_uLCDloops)
	_uLCDloops--;
}

void LCDClear()
{
	_uLCDloops = LCD_S_INSTR;
	PMADDR = 0x0000;
	PMDIN1 = 0b00000001;
	while(_uLCDloops)
	_uLCDloops--;
}

void LCDPut(char A)
{
	_uLCDloops = LCD_F_INSTR;
	PMADDR = 0x0001;
	PMDIN1 = A;
	while(_uLCDloops)
	_uLCDloops--;
	Nop();
	Nop();
	Nop();
	Nop();
}

void Wait(unsigned int B)
{
	while(B)
	B--;
}



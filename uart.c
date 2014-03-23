#include "uart.h"
#include <libpic30.h>
#include <math.h>
#include <xc.h>

#define MAX_BUFF1 30
static volatile unsigned char rxData1[MAX_BUFF1];
static volatile unsigned char rxWrIndex1 = 0;
static volatile unsigned char rxRdIndex1 = 0;
static volatile unsigned char rxCounter1 = 0;

void initUart1(long baud)
{
	//U1BRG = 0x05;//za PLLx4 Xtal=11.059200 i baud=115200 => 0x05
    U1BRG = (double)FCY / (16 * baud) - 1;

    IEC0bits.U1RXIE = 1; //1 omogucavamo rx1 interupt
    IEC0bits.U1TXIE = 0; // nema interrupt na kecu

    U1MODEbits.ALTIO = 1; // ALTERNATIVNI PINOVI ZA UART

    U1STAbits.URXISEL = 0x00; // prijemni interrupt na svaki karakter
    U1STAbits.UTXISEL = 0;

    IPC2bits.U1RXIP = 0x06; // prioritet VISI

    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1; // predajnik se mora ukljuciti posle ukljucivanja modula

    rxRdIndex1 = rxWrIndex1 = rxCounter1 = 0;
    __delay_ms(100);
}

void sendUart1(unsigned char data)
{
    while(U1STAbits.TRMT == 0);
    U1TXREG = data;
}

unsigned char getUart1(void)
{
    while(rxCounter1 == 0);

    SRbits.IPL = 7;
    unsigned char data = rxData1[rxRdIndex1];
    rxCounter1--;
    SRbits.IPL = 0;

    if(++rxRdIndex1 == MAX_BUFF1)
        rxRdIndex1 = 0;

    return data;
}
// na bikon
static unsigned char status;
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;

    unsigned char data = U1RXREG;
    status = U1STAbits.PERR | U1STAbits.FERR | U1STAbits.OERR;
    // provera greske na prijemu
    if(status == 0)
    {
        rxData1[rxWrIndex1] = data;

        if(++rxWrIndex1 == MAX_BUFF1)
            rxWrIndex1 = 0;

        ++rxCounter1;
    }

    U1STAbits.OERR = 0;
}

 // ***************************   UART2   ****************************
/*
#define MAX_BUFF2 10
static volatile unsigned char rxData2[MAX_BUFF2];
static volatile unsigned char rxWrIndex2 = 0;
static volatile unsigned char rxRdIndex2 = 0;

// komunikacija sa glavnom
void initUart2(long baud)
{
    //U2BRG = 0x05; // baud == 115200
    U2BRG = (double)FCY / (16 * baud) - 1;
    IEC1bits.U2RXIE = 1;//omogucavamo rx1 interupt
    U2STAbits.URXISEL = 0; // interrupt na svaki primljeni karakter

    IPC6bits.U2RXIP = 0x02; // prioritet visi

    U2MODEbits.UARTEN = 1;//ukljucujemo ovaj modul

    U2STAbits.UTXEN = 1;//ukljucujemo predaju

    __delay_ms(100);
}

unsigned char getUart2(void)
{
    while(rxWrIndex1 == rxRdIndex1);

    unsigned char data = rxData1[rxRdIndex1];

    if(++rxRdIndex1 == MAX_BUFF2)
        rxRdIndex1 = 0;

    return data;
}

void sendUart2(unsigned char data)
{
    while(U2STAbits.UTXBF);
    //while(U2STAbits.TRMT == 0);
    U2TXREG = data;
}

// na raspberry
void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;
    unsigned char data = U2RXREG;

    // provera greske na prijemu
    if((U2STAbits.PERR | U2STAbits.FERR | U2STAbits.OERR) == 0)
    {
        reportCoord();  
    }

    U2STAbits.OERR = 0;
}


*/
void flush(void)
{
    unsigned char i;
    rxRdIndex1 = rxWrIndex1 = rxCounter1 = 0;
 //   rxRdIndex2 = rxWrIndex2 = 0;
    for(i = 0; i < MAX_BUFF1; ++i)
	rxData1[i] = 0;
}

unsigned char UART1_checkRx(void)
{
    return rxCounter1;
}
#include "upravljanje.h"
#include "uart.h"
#include <libpic30.h>
#include <xc.h>
#include "modbus_slave.h"

static int bigX, bigY;
static int smallX, smallY;

#define indikacija LATCbits.LATC14

char started(void)
{
    char tempCoilStatus;
    ModbusGetCoil(MODBUS_START_COIL, &tempCoilStatus);

    return tempCoilStatus;
}

void DirFunc(int dir)
{
    SMER_485 = dir;
}

void getCoord(char id, volatile int *x, volatile int *y)
{
    unsigned char c1, c2;

    do
    {
        flush();
        sendUart1(id);
        sendUart1('F');
        sendUart1('E');

        __delay_ms(60);
    }while(UART1_checkRx() < 4);

    c1 = getUart1();
    c2 = getUart1();
    *x = (c1 << 8) | (c2 & 0xFF);

    c1 = getUart1();
    c2 = getUart1();
    *y = (c1 << 8) | (c2 & 0xFF);
}


void executor(void)
{
    char numOfRobots;

    ModbusGetCoil(MODBUS_ROBOTS_COIL, &numOfRobots);

    sendUart1('S');
    sendUart1('S');
    sendUart1('S');
    
    while(1)
    {
        getCoord('V', &bigX, &bigY); // prozivanje velikog bikona

        SRbits.IPL = 7;

        ModbusSetRegister(REG_V_X, bigX);
        ModbusSetRegister(REG_V_Y, bigY);

        SRbits.IPL = 0;

        if(numOfRobots == 1) // numOfRobots je binarni broj- 0 -> jedan robot, 1 -> dva robota
        {
            getCoord('M', &smallX, &smallY); // prozivanje malog bikona

            SRbits.IPL = 7;

            ModbusSetRegister(REG_M_X, smallX);
            ModbusSetRegister(REG_M_Y, smallY);

            SRbits.IPL = 0;
        }
    }
}
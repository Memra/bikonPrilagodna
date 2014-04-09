#include "upravljanje.h"
#include "uart.h"
#include <libpic30.h>
#include <xc.h>
#include "modbus_slave.h"


void DirFunc(int dir)
{
    SMER_485 = dir;
}

void executor(void)
{
    unsigned char i = 0;
    unsigned char rxBuffer[3];
    
    unsigned int x, y;
    while(1)
    {
        rxBuffer[2] = rxBuffer[1];
        rxBuffer[1] = rxBuffer[0];
        rxBuffer[0] = getUart1();

        if((rxBuffer[0] == 'V' || rxBuffer[1] == 'M') && rxBuffer[1] == 'F' && rxBuffer[2] == 'E')
        {
            x = getUart1();
            x = (x << 8) | getUart1();
            y = getUart1();
            y = (y << 8) | getUart1();

             SRbits.IPL = 7;

             if(rxBuffer[0] == 'V')
             {
                ModbusSetRegister(REG_V_X, x);
                ModbusSetRegister(REG_V_Y, y);
             }
             else
             {
                ModbusSetRegister(REG_M_X, x);
                ModbusSetRegister(REG_M_Y, y);
             }

             SRbits.IPL = 0;
        }
    }
}
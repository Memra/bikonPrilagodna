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
    int x, y;
    unsigned char rx[7];
    unsigned char i = 0;

    while(1)
    {
        for(i = 5; i >= 0; --i)
            rx[i + 1] = rx[i];

        rx[0] = getUart1();
        if((rx[6] == 'M' || rx[6] == 'V') && rx[5] == 'F' && rx[4] == 'E')
        {
            if((rx[3] == 'V' || rx[3] == 'M') && rx[2] == 'F' && rx[1] == 'E')
                continue;

            x = (rx[3] << 8) | rx[2];
            y = (rx[1] << 8) | rx[0];

            SRbits.IPL = 7;
            if(rx[6] == 'M')
            {
                ModbusSetRegister(REG_M_X, x);
                ModbusSetRegister(REG_M_Y, y);
            }

            if(rx[6] == 'V')
            {
                ModbusSetRegister(REG_V_X, x);
                ModbusSetRegister(REG_V_Y, y);
            }
            SRbits.IPL = 0;
        }
    }
}
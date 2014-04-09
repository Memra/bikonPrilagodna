#define FCY 11059200ULL

#include "uart.h"
#include "upravljanje.h"
#include <libpic30.h>
#include <xc.h>
#include "modbus_slave.h"

_FWDT(WDT_OFF);
_FOSC(CSW_FSCM_OFF & XT_PLL4); // preskaler == 4, fosc = 11059200 / 4 -> fcy == fosc

int main(void)
{
    TRISCbits.TRISC14 = 0;
    TRISFbits.TRISF0 = 0;
    INTCON1bits.NSTDIS = 0;
    ModbusSlaveInit(2, DirFunc);

    initUart1(115200); // ka xbee

    __delay_ms(500);

    executor(); // pisanje po sirotinji
    
    return 0;
}


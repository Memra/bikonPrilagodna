#ifndef MODBUS_SLAVE_CFG_H
#define MODBUS_SLAVE_CFG_H

//#define _ATMEGA8_ // _ATMEGA8_, _ATMEGA16_, _ATMEGA32_, _ATMEGA64_, _ATMEGA128_
//#define _UART0_ // _UART0_, _UART1_ (only for _ATMEGA64_ and _ATMEGA128_)

#define _DSPIC30F411_
#define _UART2_ // _UART1_, _UART2_ (for _DSPIC30F411_ and _DSPIC30F412_)

#define MODBUS_OSC 11059200ULL
#define MODBUS_BAUDRATE 115200

#define MODBUS_USE_INTERRUPT 1

#define MODBUS_USE_COILS 1
#define MODBUS_NUM_COILS 1

#define MODBUS_USE_REGISTERS 1
#define MODBUS_NUM_REGISTERS 5

#define MODBUS_USE_EVENTS 0
#define MODBUS_NUM_EVENTS 1


#endif

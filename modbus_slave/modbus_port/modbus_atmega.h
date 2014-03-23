#ifndef MODBUS_ATMEGA_H
#define MODBUS_ATMEGA_H

#include "modbus_slave_cfg.h"

void ModbusUartInit(void (*DirPinFunc)(int), void (*ModbusMainFunc)(void));
int ModbusUartGetChar(char *data);
int ModbusUartPutChar(char c);
int ModbusUartPutString(char *s, unsigned int count);

#endif

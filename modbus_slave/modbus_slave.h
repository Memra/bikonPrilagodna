#ifndef MB_SLAVE_H
#define MB_SLAVE_H

#include "modbus_slave_cfg.h"

#ifndef MODBUS_NUM_COILS
#define MODBUS_NUM_COILS 2
#endif
#ifndef MODBUS_NUM_REGISTERS
#define MODBUS_NUM_REGISTERS 4
#endif

#if (MODBUS_USE_EVENTS == 1)
void ModbusEventDispatcher();
#endif

void ModbusSlaveInit(unsigned char address, void (*DirPinFunc)(int));
void ModbusSlaveMain();

#if (MODBUS_USE_COILS == 1)
int ModbusSetCoil(unsigned short CoilID, char value);
int ModbusSetCoilPrivate(unsigned short CoilID, char value);
int ModbusGetCoil(unsigned short CoilID, char *value);
int ModbusGetMultipleCoils(unsigned short CoilAddr, unsigned int count, unsigned char *data);
#if (MODBUS_USE_EVENTS == 1)
int ModbusSetCoilChangeEvent(unsigned int CoilAddr, void (*CoilEventFunc)(unsigned int));
#endif
#endif
#if (MODBUS_USE_REGISTERS == 1)
int ModbusSetRegister(unsigned short RegisterID, short value);
int ModbusSetRegisterPrivate(unsigned short RegisterID, short value);
int ModbusGetRegister(unsigned short RegisterID, short *value);
int ModbusGetMultipleRegisters(unsigned short RegAddr, unsigned int count, unsigned short *value);
#if (MODBUS_USE_EVENTS == 1)
int ModbusSetRegisterChangeEvent(unsigned int RegAddr, void (*RegisterEventFunc)(unsigned int));
#endif
#endif

#endif

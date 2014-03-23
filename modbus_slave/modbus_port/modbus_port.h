#ifndef MODBUS_PORT_H
#define MODBUS_PORT_H

#if defined _ATMEGA8_ || defined _ATMEGA16_ || defined _ATMEGA32_ || \
                         defined _ATMEGA64_ || defined _ATMEGA128_
#include "modbus_atmega.h"
#elif defined _DSPIC30F4011_ || _DSPIC30F412_
#include "modbus_dspic.h"
#endif

#endif

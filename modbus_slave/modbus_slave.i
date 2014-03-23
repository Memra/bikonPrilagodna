
void ModbusEventDispatcher();

void ModbusSlaveInit(unsigned char address, void (*DirPinFunc)(int));
void ModbusSlaveMain();

int ModbusSetCoil(unsigned short CoilID, char value);
int ModbusSetCoilPrivate(unsigned short CoilID, char value);
int ModbusGetCoil(unsigned short CoilID, char *value);
int ModbusGetMultipleCoils(unsigned short CoilAddr, unsigned int count, unsigned char *data);
int ModbusSetCoilChangeEvent(unsigned int CoilAddr, void (*CoilEventFunc)(unsigned int));
int ModbusSetRegister(unsigned short RegisterID, short value);
int ModbusSetRegisterPrivate(unsigned short RegisterID, short value);
int ModbusGetRegister(unsigned short RegisterID, short *value);
int ModbusGetMultipleRegisters(unsigned short RegAddr, unsigned int count, unsigned short *value);
int ModbusSetRegisterChangeEvent(unsigned int RegAddr, void (*RegisterEventFunc)(unsigned int));

void ModbusUartInit(void (*DirPinFunc)(int), void (*ModbusMainFunc)(void));
int ModbusUartGetChar(char *data);
int ModbusUartPutChar(char c);
int ModbusUartPutString(char *s, unsigned int count);

static char slave_address;

static char coils_array[6];
static short registers_array[9];

static unsigned char events_array[1];
static unsigned char events_num;
static unsigned char events_head;
static unsigned char events_tail;
static void (*CoilsEventFunctions[6])(unsigned int);
static void (*RegistersEventFunctions[9])(unsigned int);
int modbus_state = 0;  
char asciiPart[4];
int i;
int k;
int crlf;
int broadcast;
unsigned char function;
unsigned char data[80];
unsigned char byte[40];
unsigned char LRC;
int ModbusSetCoil(unsigned short CoilID, char value)
{
if(CoilID >= 6)
{
return -1;
}
coils_array[CoilID] = value;
return 1;
}
int ModbusSetCoilPrivate(unsigned short CoilID, char value)
{
if(CoilID >= 6)
{
return -1;
}
coils_array[CoilID] = value;
if(CoilID < 64 && CoilsEventFunctions[CoilID] != 0)
{
events_array[events_head] = (0x00<<6) | CoilID;
if(++events_head == 1) events_head = 0;
events_num++;

}
return 1;
}
int ModbusGetCoil(unsigned short CoilID, char *value)
{
if(CoilID >= 6)
{
return -1;
}
if(value == 0)
{
return -2;
}
*value = coils_array[CoilID];
return 1;
}
int ModbusGetMultipleCoils(unsigned short CoilAddr, unsigned int count, unsigned char *data)
{
int i;
if(CoilAddr + count > 6 || count == 0)
{
return -1;
}
if(data == 0)
{
return -2;
}
for(i = 0; i < ((count-1)>>3)+1; i++)
{
data[i] = 0;
}
for(i = 0; i < count; i++)
{
if(coils_array[CoilAddr+i])
{
data[i>>3] |= 1<<(i%8);
}
else
{
data[i>>3] &= ~(1<<(i%8));
}
}
return ((count-1)>>3)+1;
}
int ModbusSetCoilChangeEvent(unsigned int CoilAddr, void (*CoilEventFunc)(unsigned int))
{
if(CoilAddr < 64)
{
CoilsEventFunctions[CoilAddr] = CoilEventFunc;
return 1;
}
return 0;
}
int ModbusSetRegister(unsigned short RegisterID, short value)
{
if(RegisterID >= 9)
{
return -1;
}
registers_array[RegisterID] = value;
return 1;
}
int ModbusSetRegisterPrivate(unsigned short RegisterID, short value)
{
if(RegisterID >= 9)
{
return -1;
}
registers_array[RegisterID] = value;
if(RegisterID < 64 && RegistersEventFunctions[RegisterID] != 0)
{
events_array[events_head] = (0x01<<6) | RegisterID;
if(++events_head == 1) events_head = 0;
events_num++;

} 
return 1;
}
int ModbusGetRegister(unsigned short RegisterID, short *value)
{
if(RegisterID >= 9)
{
return -1;
}
*value = registers_array[RegisterID];
return 1;
}
int ModbusGetMultipleRegisters(unsigned short RegAddr, unsigned int count, unsigned short *value)
{
int i;
if(RegAddr + count > 9 || count == 0)
{
return -1;
}
for(i = 0; i < count; i++)
{
value[i] = registers_array[RegAddr+i];
}
return i;
}
int ModbusGetMultipleRegistersBE(unsigned short RegAddr, unsigned int count, unsigned short *value)
{
int i;
unsigned char *pvalue;
pvalue = (unsigned char *)value;
if(RegAddr + count > 9 && count == 0)
{
return -1;
}
for(i = 0; i < count; i++)
{
pvalue[i*2] = registers_array[RegAddr+i]>>8;
pvalue[i*2+1] = registers_array[RegAddr+i]&0xFF;
}
return i;
}
int ModbusSetRegisterChangeEvent(unsigned int RegAddr, void (*RegisterEventFunc)(unsigned int))
{
if(RegAddr < 64)
{
RegistersEventFunctions[RegAddr] = RegisterEventFunc;
return 1;
}
return 0;
}
unsigned int ModbusAccumulate(unsigned char *data, unsigned int count)
{
int i;
unsigned int accum = 0;
for(i = 0; i < count; i++)
{
accum += data[i];
}
return accum;
}
int ModbusByteToASCII(unsigned char Byte, unsigned char *Nibble1, unsigned char *Nibble2)
{
if( (Nibble1 == 0) || (Nibble2 == 0) )
{
return -1;
}
*Nibble1 = (Byte >> 4) + 48;
*Nibble2 = (Byte & 0x0F) + 48;
if(*Nibble1 > 57)
{
*Nibble1 += 7;
}
if(*Nibble2 > 57)
{
*Nibble2 += 7;
}
return 1;
}
int ModbusASCIIToByte(unsigned char *Byte, char Nibble1, char Nibble2)
{
if( Byte == 0 )
{
return -1;
}
if(Nibble1 > 57)
{
Nibble1 -= 7;
}
if(Nibble2 > 57)
{
Nibble2 -= 7;
}
Nibble1 -= 48;
Nibble2 -= 48;
*Byte = (unsigned char)((Nibble1<<4) | Nibble2);
return 1;
}
int ModbusAsciify(unsigned char *byteMessage, unsigned char *asciiMessage, int byteMessageSize)
{
int i;
if( (byteMessage == 0) || (asciiMessage == 0) )
{
return -1;
}
for(i=0; i < byteMessageSize; i++)
{
ModbusByteToASCII(byteMessage[i], &asciiMessage[i*2], &asciiMessage[i*2+1]);
}
return byteMessageSize*2;
}
void ModbusSlaveInit(unsigned char address, void (*DirPinFunc)(int))
{
int i;
for(i = 0; i < 1; i++)
{
events_array[i] = 0;
}
events_num = 0;
events_head = 0;
events_tail = 0;
for(i = 0; i < 6; i++)
{
CoilsEventFunctions[i] = 0;
}                      
for(i = 0; i < 9; i++)
{
RegistersEventFunctions[i] = 0;
}
slave_address = address;
broadcast = 0;
ModbusUartInit(DirPinFunc, 1?ModbusSlaveMain:0);
}
void ModbusSlaveMain()
{
char recv;
unsigned char recvLRC;
while(ModbusUartGetChar(&recv))
{
switch(modbus_state)
{
case 0:
if(recv == ':')
{
modbus_state = 1;
i = 0;
LRC = 0;
}
break;
case 1:
asciiPart[i++] = recv;
if(recv == ':')
{
modbus_state = 1;
i = 0;
LRC = 0;
}
else if(i == 2)
{
ModbusASCIIToByte(&byte[0], asciiPart[0], asciiPart[1]);
if(slave_address == byte[0])
{
modbus_state = 2;
LRC += byte[0];
i = 0;
}
else if(byte[0] == 0)
{
modbus_state = 2;
LRC += byte[0];
i = 0;
broadcast = 1;
}
else
{
modbus_state = 0;
}
}
break;
case 2:
asciiPart[i++] = recv;
if(recv == ':')
{
modbus_state = 1;
i = 0;
LRC = 0;
}
else if(i == 2)
{
ModbusASCIIToByte(&byte[0], asciiPart[0], asciiPart[1]);
function = byte[0];
modbus_state = 3;
LRC += byte[0];
i = 0;
crlf = 0;
k = 0;
}
break;
case 3:
if(recv == '\r')
{
crlf = 1;
}
else if(recv == '\n' && crlf == 1)
{
crlf = 0;
recvLRC = byte[i-1];
LRC += (unsigned char)ModbusAccumulate(byte, i-1);
LRC = (LRC ^ 0xFF)+1;
if(LRC != recvLRC)
{
modbus_state = 0;
break;
}
else
{
goto start_function;
}
}
else if(recv == ':')
{
modbus_state = 1;
i = 0;
LRC = 0;
}
else
{
data[k] = recv;
k++;
if(k == 2)
{
ModbusASCIIToByte(&byte[i], data[0], data[1]);
i++;
k = 0;
}
}
break;
start_function:
switch(function)
{
case 1:
if(byte[0]*256+byte[1] + byte[2]*256+byte[3] > 6 || byte[2]*256+byte[3] == 0)
{
modbus_state = 0;
break;
}
byte[0] = ModbusGetMultipleCoils(byte[0]*256+byte[1], byte[2]*256+byte[3], &byte[1]);
i = byte[0]+1;
break;
case 5:
if(byte[2] == 0xFF && byte[3] == 0x00)
{
if(ModbusSetCoilPrivate(byte[0]*256+byte[1], 1) == -1)
{
modbus_state = 0;
}
}
else if(byte[2] == 0x00 && byte[3] == 0x00)
{
if(ModbusSetCoilPrivate(byte[0]*256+byte[1], 0) == -1)
{
modbus_state = 0;
}
}
else
{
modbus_state = 0;
}
i = 4;
break;
case 15:
if(byte[0]*256+byte[1] + byte[2]*256+byte[3] > 6 || byte[2]*256+byte[3] == 0)
{
modbus_state = 0;
break;
}
for(k = 0; k < byte[2]*256+byte[3]; k++)
{
ModbusSetCoilPrivate(byte[0]*256+byte[1]+k, (byte[5+(k>>3)]&(1<<(k%8)))?1:0);
}
i = 4;
break;
case 3:
if(byte[0]*256+byte[1] + byte[2]*256+byte[3] > 9 || byte[2]*256+byte[3] == 0)
{
modbus_state = 0;
break;
}
byte[0] = ModbusGetMultipleRegistersBE(byte[0]*256+byte[1], byte[2]*256+byte[3], (unsigned short*)&byte[1])*2;
i = byte[0]+1;
break;
case 6:
if(ModbusSetRegisterPrivate(byte[0]*256+byte[1], byte[2]*256+byte[3]) == -1)
{
modbus_state = 0;
}
i = 4;
break;
case 16:
if(byte[0]*256+byte[1] + byte[2]*256+byte[3] > 9 || byte[2]*256+byte[3] == 0)
{
modbus_state = 0;
break;
}
for(k = 0; k < byte[2]*256+byte[3]; k++)
{
ModbusSetRegisterPrivate(byte[0]*256+byte[1]+k, byte[5+k*2]*256+byte[6+k*2]);
}
i = 4;
break;
default:
modbus_state = 0;
break;
}
if(modbus_state == 3 && !broadcast)
{
data[0] = ':';
ModbusByteToASCII(slave_address, &data[1], &data[2]);
ModbusByteToASCII(function, &data[3], &data[4]);
ModbusAsciify(byte, &data[5], i);
LRC = ModbusAccumulate(byte, i);
LRC += slave_address;
LRC += function;
LRC = (LRC ^ 0xFF)+1;
ModbusByteToASCII(LRC, &data[i*2+5], &data[i*2+6]);
data[i*2+7] = '\r';
data[i*2+8] = '\n';
ModbusUartPutString(data, i*2+9);
modbus_state = 0;
}
broadcast = 0;
break;
default:
modbus_state = 0;
break;
}
}
}
void ModbusEventDispatcher()
{
unsigned char event;
while(events_num)
{
event = events_array[events_tail];
events_num--;
if(++events_tail == 1) events_tail = 0;
switch(event>>6)
{
case 0x00:
(*CoilsEventFunctions[event&0x3f])(event&0x3F);
break;
case 0x01:
(*RegistersEventFunctions[event&0x3f])(event&0x3F);
break;
default:
break;
}
}
}

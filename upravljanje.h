#ifndef _UPRAVLJANJE_H
#define _UPRAVLJANJE_H

#ifndef FCY
#define FCY 11059200ULL
#endif

#define SMER_485 LATFbits.LATF0

#define REG_V_X 1
#define REG_V_Y 2
#define REG_M_X 3
#define REG_M_Y 4

void getCoord(char, volatile int *, volatile int *);
void executor(void);
void DirFunc(int dir);
char started(void);

#endif
#ifndef TTT_H
#define TTT_H

#include <stdint.h>


typedef enum
{
    TTT_START,
    TTT_RUNNING,
    TTT_PAUSE,
    TTT_STOP
} TTT_StateTypeDef;

int TTT_Ges(void);
int TTT_CheckWin(void);
int TTT_SetValue(void);
int TTT_SetPos(uint32_t x, uint32_t y);

#endif

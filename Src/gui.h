#ifndef GUI_H
#define GUI_H

#include <stdint.h>


void GUI_Init(void);
void GUI_Del(char winner);
char GUI_GetKeyPress(void);
void GUI_SetCursorPosition(int8_t x, int8_t y);
void GUI_SetValue(int8_t x, int8_t y, uint8_t isPlayerX);

#endif

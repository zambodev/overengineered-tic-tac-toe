#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "gui.h"


#define GUI_CROSS       0x254B
#define GUI_VBAR        0x2503
#define GUI_HBAR        0x2501
#define GUI_VBAR_CL     0x2523
#define GUI_VBAR_CR     0x252B


/******************************** Private structures and data types ********************************/

struct termios TIO_Old;
struct termios TIO_New;

/******************************** Private functions ********************************/

static void GUI_Draw(void)
{
    wprintf(L"%3s%lc%3s%lc\n \
            \r%lc%lc%lc%lc%lc%lc%lc%lc%lc%lc%lc\n \
            \r%3s%lc%3s%lc\n \
            \r%lc%lc%lc%lc%lc%lc%lc%lc%lc%lc%lc\n \
            \r%3s%lc%3s%lc\n",
            " ", GUI_VBAR, " ", GUI_VBAR,
            GUI_HBAR, GUI_HBAR, GUI_HBAR, GUI_CROSS, GUI_HBAR, GUI_HBAR, GUI_HBAR, GUI_CROSS, GUI_HBAR, GUI_HBAR, GUI_HBAR,
            " ", GUI_VBAR, " ", GUI_VBAR,
            GUI_HBAR, GUI_HBAR, GUI_HBAR, GUI_CROSS, GUI_HBAR, GUI_HBAR, GUI_HBAR, GUI_CROSS, GUI_HBAR, GUI_HBAR, GUI_HBAR,
            " ", GUI_VBAR, " ", GUI_VBAR
    );
}

/******************************** Public functions ********************************/

void GUI_Init(void)
{
    setlocale(LC_CTYPE, "");
    
    tcgetattr(STDIN_FILENO, &TIO_Old);
    memcpy(&TIO_New, &TIO_Old, sizeof(TIO_Old));
    TIO_New.c_lflag &=(~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &TIO_New);

    wprintf(L"\x1b[2J\x1b[H");
    GUI_Draw();
    GUI_SetCursorPosition(0, 0);
    fflush(stdout);
}

void GUI_Del(char winner)
{
    if(winner != ' ')
        wprintf(L"\x1B[6;0HPlayer %c win!\n", winner);
    else
        wprintf(L"\x1B[6;0HIt's a tie!\n");
    
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &TIO_Old);
}

void GUI_SetCursorPosition(int8_t x, int8_t y)
{
    int xpos = 2 + x * 4;
    int ypos = 1 + y * 2;

    wprintf(L"\x1B[%d;%dH", ypos, xpos);
    fflush(stdout);
}

char GUI_GetKeyPress(void)
{
	unsigned char value;
	static fd_set rfds;
	static struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 200;

    if(select(1, &rfds, NULL, NULL, &tv) <= 0)
        return -1;

    value = getchar();
    if(value == '\033')
    {
        getchar();
        value = getchar();
    }

    return value;
}

void GUI_SetValue(int8_t x, int8_t y, uint8_t isPlayerX)
{
    if(isPlayerX)
        wprintf(L"X\x1B[1D");
    else
        wprintf(L"O\x1B[1D");

    fflush(stdout);
}
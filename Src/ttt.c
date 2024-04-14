#include "ttt.h"
#include "gui.h"

/* 32 bitmask for game data
 *      T   P S Y X Game data
 *  31| |   | | | | |                |0
 *    00000000000000000000000000000000            
 * First 18 bits are used for game board values
 * Bits 18 to 21 are used for X and Y
 * Bits 22 to 23 are used for game state
 *  - 00: Start
 *  - 01: Run
 *  - 10: Pause
 *  - 11: End
 * Bit 24 to 25 is used to set whitch player is playing
 * Bit 26 to 29 are used as turn counter
 * Last 2 bits are unused for now
*/

/* Private variables */
static uint32_t GameData;
static const uint32_t wincasesX[8] = {
    0x00030303U,
    0x00003330U,
    0x0003F000U,
    0x00000FC0U,
    0x0000003FU,
    0x00030C30U,
    0x0000C30CU,
    0x000030C3U
};
static const uint32_t wincasesY[8] = {
    0x00020202U,
    0x00002220U,
    0x0002A000U,
    0x00000A80U,
    0x0000002AU,
    0x00020820U,
    0x00008208U,
    0x00002082U
};  

int TTT_Ges(void)
{
    switch((GameData & 0xC00000U) >> 0x16U)
    {
        // Start
        case 0x00:
        {
            GUI_Init();
            // Go into Run state
            GameData |= 0x1400000U;
        } break;
        // Run
        case 0x01:
        {
            // Get X and Y
            int8_t x = ((GameData & 0x0C0000U) >> 0x12U);
            int8_t y = ((GameData & 0x300000U) >> 0x14U);
            // Wait till key is pressed
            char key = GUI_WaitKeyPress();

            switch(key)
            {
                case 66:
                case 'w':
                {
                    ++y;
                    if(y > 2)
                        y = 0;
                    else if(y < 0)
                        y = 2; 
                } break;
                case 65:
                case 's':
                {
                    --y;
                    if(y > 2)
                        y = 0;
                    else if(y < 0)
                        y = 2;
                } break;
                case 67:
                case 'd':
                {
                    ++x;
                    if(x > 2)
                        x = 0;
                    else if(x < 0)
                        x = 2;
                } break;
                case 68:
                case 'a':
                {
                    --x;
                    if(x > 2)
                        x = 0;
                    else if(x < 0)
                        x = 2;
                } break;
                case 32:
                case 10:
                {
                    TTT_SetValue();
                        
                    // Increase game turn counter
                    uint8_t turnCount = (GameData & 0x3C000000U) >> 0x1AU;
                    ++turnCount;

                    if(turnCount == 9)
                    {
                        // Go into Stop state
                        GameData |= 0xC00000U;
                        return 0;
                    }

                    GameData &= 0xC3FFFFFFU;
                    GameData |= (turnCount << 0x1AU);
                } break;
                default:
                {
                    return 0;
                } break;
            }

            // Update cursor position
            TTT_SetPos(x, y);
            GUI_SetCursorPosition(x, y);

            // Check if there is a winner
            if(TTT_CheckWin() != 0)
                // Go into Stop state
                GameData |= 0xC00000U;
        } break;
        // Pause
        case 0x02:
        {
            // TODO
        } break;
        // Stop
        case 0x03:
        {   
            char winner;
            if(((GameData & 0x03000000U) >> 0x18U) == 0x03U)
                winner = 'X';
            else if(((GameData & 0x02000000U) >> 0x18U) == 0x02U)
                winner = 'O';
            else
                winner = ' ';

            GUI_Del(winner);
            GameData = 0x00U;
            return -1;
        } break;
    }

    return 0;
}

int TTT_CheckWin(void)
{
    for(int i = 0; i < 8; ++i)
    {
        uint32_t res = GameData & wincasesX[i];

        if(res == wincasesX[i])
        {
            GameData |= 0x03000000U;
            return 1;
        }
        else if(res == wincasesY[i])
        {
            GameData &= 0xFCFFFFFFU;
            GameData |= 0x02000000U;
            return 1;
        }
    }

    return 0;
}

int TTT_SetValue(void)
{
    // Get X and Y
    int8_t x = ((GameData & 0x0C0000U) >> 0x12U);
    int8_t y = ((GameData & 0x300000U) >> 0x14U);

    // Check if the spot is free
    if((GameData & (0b10U << ((y * 3 + x) * 2))) != 0)
        return 0;

    GUI_SetValue(x, y, ((GameData & 0x01000000U) >> 18));

    // Check which player has made the move
    if((GameData & 0x01000000U) != 0x00U)
    {
        GameData |= (0x03U << ((y * 3 + x) * 2));
        // Set other player turn
        GameData &= 0xFEFFFFFFU;
        return 1;
    }
    else
    { 
        GameData |= (0x02U << ((y * 3 + x) * 2));
        GameData |= 0x01000000U;
        
        return -1;
    }
}

int TTT_SetPos(uint32_t x, uint32_t y)
{
    if(x > 2 || x < 0 || y > 2 || y < 0)
        return -1;

    GameData &= 0xFFC3FFFFU;
    GameData |= ((y << 20) | (x << 18));
    
    return 0;
}

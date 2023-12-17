#ifndef DEBUG_H
#define DEBUG_H

#include <Windows.h>

inline void DebugClearScreen(HANDLE Console)
{
    COORD CoordScreen = {};
    DWORD CharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO CSBI;
    DWORD ConSize;

    if (!GetConsoleScreenBufferInfo(Console, &CSBI))
    {
        return;
    }
    ConSize = CSBI.dwSize.X * CSBI.dwSize.Y;

    if (!FillConsoleOutputCharacter(Console, ' ', ConSize, CoordScreen, &CharsWritten))
    {
        return;
    }

    if (!GetConsoleScreenBufferInfo(Console, &CSBI))
    {
        return;
    }

    if (!FillConsoleOutputAttribute(Console, CSBI.wAttributes, ConSize, CoordScreen, &CharsWritten))
    {
        return;
    }

    SetConsoleCursorPosition(Console, CoordScreen);
}

enum debug_colours
{
    DB_Default = 15,
    DB_Highlight = 199,
    DB_Dim = 159,
};

inline void DebugSetColour(HANDLE Console, WORD Attributes)
{
    SetConsoleTextAttribute(Console, Attributes);
}

#endif

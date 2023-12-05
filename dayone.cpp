// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>

#include "common/common.h"

struct number
{
    char* Text;
    u32 Len;
    u32 Value;
};

// @formatter:off
struct number Numbers[] = {
    {"one", 3, 1},
    {"two", 3, 2},
    {"three", 5, 3},
    {"four", 4, 4},
    {"five", 4, 5},
    {"six", 3, 6},
    {"seven", 5, 7},
    {"eight", 5, 8},
    {"nine", 4, 9}
};
// @formatter:on

u32 GetNumber(char C)
{
    u32 Result = C - '0';
    return Result;
}

u32 GetNumberMatch(char* At)
{
    u32 Result = 0;
    for (int Index = 0; Index < ArrayCount(Numbers); ++Index)
    {
        number* Test = Numbers + Index;
        if (StringsAreEqual(Test->Len, Test->Text, Test->Len, At))
        {
            Result = Test->Value;
            break;
        }
    }

    if (Result == 0 && IsNumber(*At))
    {
        Result = GetNumber(*At);
    }
    return Result;
}

s32 main()
{
    FILE* File = fopen("inputs/dayone.txt", "rb+");
    if (File)
    {
        fseek(File, 0, SEEK_END);
        int Count = ftell(File);
        fseek(File, 0, SEEK_SET);

        char* Data = (char*)malloc(Count);
        fread(Data, Count, 1, File);

        u32 Sum = 0;
        u32 First = 0;
        u32 Last = 0;
        bool ResetLine = true;
        char* At = Data;
        while (At != &Data[Count])
        {
            u32 NumberMatch = GetNumberMatch(At);
            if (NumberMatch)
            {
                if (ResetLine)
                {
                    First = NumberMatch;
                    Last = NumberMatch;
                    ResetLine = false;
                }
                else
                {
                    Last = NumberMatch;
                }
            }
            else if (!ResetLine && IsEndOfLine(*At))
            {
                printf("Line result: %u%u\n", First, Last);
                ResetLine = true;
                First *= 10;
                Sum += First;
                Sum += Last;
                First = 0;
                Last = 0;
            }
            *At++;
        }

        printf("The sum is: %u", Sum);
    }
    return 0;
}

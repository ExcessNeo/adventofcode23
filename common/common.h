#pragma once

#include <cstdint>

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t s32;

typedef s32 b32;
typedef uintptr_t umm;

struct buffer
{
    umm Count;
    u8* Data;
};

typedef buffer string;

u8* GetDataFromFile(char* Filename, umm* Count);

inline string BundleString(umm Count, char* Z)
{
    string Result;

    Result.Count = Count;
    Result.Data = (u8 *)Z;

    return Result;
}

inline s32 StringLength(char* String)
{
    s32 Count = 0;
    if (String)
    {
        while (*String++)
        {
            ++Count;
        }
    }
    return Count;
}

inline string WrapZ(char* Z)
{
    string Result;

    Result.Count = StringLength(Z);
    Result.Data = (u8*)Z;

    return Result;
}

#define BundleZ(z) BundleString(sizeof(z) - 1, (char*)(z))

inline u8* Advance(buffer* Buffer, umm Count)
{
    u8* Result = nullptr;
    if (Buffer->Count >= Count)
    {
        Result = Buffer->Data;
        Buffer->Data += Count;
        Buffer->Count -= Count;
    }
    else
    {
        Buffer->Data += Buffer->Count;
        Buffer->Count = 0;
    }

    return Result;
}

inline b32 IsAlpha(char C)
{
    b32 Result = (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z');
    return Result;
}

inline bool IsNumber(char C)
{
    bool Result = C >= '0' && C <= '9';
    return Result;
}

inline bool IsEndOfLine(char C)
{
    bool Result = C == '\n' || C == '\r';
    return Result;
}

inline b32 IsSpacing(char C)
{
    b32 Result = (C == ' ' || C == '\t' || C == '\v' || C == '\f');

    return Result;
}

inline b32 IsWhitespace(char C)
{
    b32 Result = (IsSpacing(C) || IsEndOfLine(C));

    return Result;
}

inline char ToLowercase(char C)
{
    char Result = C;

    if (Result >= 'A' && Result <= 'Z')
    {
        Result += 'a' - 'A';
    }

    return Result;
}

inline b32 StringsAreEqual(umm ALength, char* A, umm BLength, char* B)
{
    b32 Result = (ALength == BLength);

    if (Result)
    {
        Result = true;
        for (u32 Index = 0; Index < ALength; ++Index)
        {
            if (A[Index] != B[Index])
            {
                Result = false;
                break;
            }
        }
    }

    return Result;
}

inline b32 StringsAreEqual(umm ALength, char* A, char* B)
{
    b32 Result;

    if (B)
    {
        char* At = B;
        for (umm Index = 0; Index < ALength; ++Index, ++At)
        {
            if (*At == 0 || A[Index] != *At)
            {
                return false;
            }
        }
        Result = (*At == 0);
    }
    else
    {
        Result = (ALength == 0);
    }

    return Result;
}

inline b32 StringsAreEqual(string A, char* B)
{
    b32 Result = StringsAreEqual(A.Count, (char *)A.Data, B);
    return Result;
}

inline b32 StringsAreEqual(string A, string B)
{
    b32 Result = StringsAreEqual(A.Count, (char *)A.Data, B.Count, (char *)B.Data);
    return Result;
}
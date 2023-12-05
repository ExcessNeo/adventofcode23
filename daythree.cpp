// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>

#include "common/common.h"
#include "common/tokenizer.h"

struct number
{
    u32 Value;
    u32 Line;
    u32 Column;
    u32 Len;
    b32 Included;
};

struct symbol
{
    u32 Line;
    u32 Column;
    b32 IsGear;
};

b32 IsPartNumber(number* Number, symbol* Symbol)
{
    b32 Result = false;
    if (Symbol->Line >= Number->Line - 1 && Symbol->Line <= Number->Line + 1)
    {
        u32 Start = Number->Column - 1;
        u32 End = Number->Column + Number->Len;
        if (Symbol->Column >= Start && Symbol->Column <= End)
        {
            Result = true;
        }
    }
    return Result;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile("inputs/daythree.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daythree.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        number Numbers[4096];
        symbol Symbols[4096];
        u32 NumCount = 0;
        u32 SymbolCount = 0;

        while (Parsing(Tokenizer))
        {
            token Token = GetToken(Tokenizer);
            if (Token.Type != Token_Period && Token.Type != Token_EndOfStream)
            {
                if (Token.Type == Token_Number)
                {
                    // printf("Num: %u\n", Token.number);
                    Numbers[NumCount].Column = Token.ColumnNumber;
                    Numbers[NumCount].Line = Token.LineNumber;
                    Numbers[NumCount].Len = Token.Text.Count;
                    Numbers[NumCount].Value = Token.Number;
                    ++NumCount;
                }
                else
                {
                    string Symbol = GetTokenTypeName(Token.Type);
                    // printf("Symbol: %.*s\n", (s32)Symbol.count, (char*)Symbol.data);
                    Symbols[SymbolCount].Column = Token.ColumnNumber;
                    Symbols[SymbolCount].Line = Token.LineNumber;
                    Symbols[SymbolCount].IsGear = Token.Type == Token_Asterisk;
                    ++SymbolCount;
                }
            }
            else if (Token.Type == Token_EndOfStream)
            {
                break;
            }
        }

        u32 PartNumSum = 0;
        for (u32 NumIndex = 0; NumIndex < NumCount; ++NumIndex)
        {
            number* Number = Numbers + NumIndex;
            for (u32 SymIndex = 0; SymIndex < SymbolCount; ++SymIndex)
            {
                symbol* Symbol = Symbols + SymIndex;

                if (!Number->Included && IsPartNumber(Number, Symbol))
                {
                    Number->Included = true;
                    PartNumSum += Number->Value;
                }
            }
        }

        u32 SumGearRatio = 0;
        for (u32 SymIndex = 0; SymIndex < SymbolCount; ++SymIndex)
        {
            symbol* Symbol = Symbols + SymIndex;

            if (Symbol->IsGear)
            {
                number* First = nullptr;
                number* Second = nullptr;
                for (u32 NumIndex = 0; NumIndex < NumCount; ++NumIndex)
                {
                    number* Number = Numbers + NumIndex;

                    if (IsPartNumber(Number, Symbol))
                    {
                        if (First == nullptr)
                        {
                            First = Number;
                        }
                        else if (Second == nullptr)
                        {
                            Second = Number;
                        }
                        else
                        {
                            printf("Third wheeler alert for Symbol at (%u:%u)\n", Symbol->Line, Symbol->Column);
                        }
                    }
                }

                if (First && Second)
                {
                    SumGearRatio += First->Value * Second->Value;
                }
            }
        }

        printf("Number count: %u\n", NumCount);
        printf("Symbol count: %u\n", SymbolCount);
        printf("Part Number Sum: %u\n", PartNumSum);
        printf("Gear ratio sum: %u\n", SumGearRatio);

        // printf("Debug included numbers:\n");
        // for (uint32 numIndex = 0; numIndex < numCount; ++numIndex)
        // {
        //     number* Number = Numbers + numIndex;
        //
        //     if (Number->included)
        //     {
        //         printf("Value: %u Line: %u Column: %u\n", Number->value, Number->line, Number->column);
        //     }
        // }
    }

    return 0;
}

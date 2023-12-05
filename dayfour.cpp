// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>

#include "common/common.h"
#include "common/tokenizer.h"

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile("inputs/dayfour.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/dayfour.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        // NOTE: 0 is left blank intentionally
        u32 Copies[256];
        Copies[0] = 0;
        for (u32 Index = 1; Index < ArrayCount(Copies); ++Index)
        {
            Copies[Index] = 1;
        }

        u32 SumPoints = 0;
        u32 TotalCards = 0;
        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                RequireIdentifier(Tokenizer, "Card");
                token CardNumber = RequireToken(Tokenizer, Token_Number);
                u32 CardIndex = CardNumber.Number;
                TotalCards = CardIndex;
                RequireToken(Tokenizer, Token_Colon);

                while (Parsing(Tokenizer))
                {
                    u32 CardCopies = Copies[CardIndex];
                    u32 WinningNumbers[10];
                    for (u32 WinIndex = 0; WinIndex < ArrayCount(WinningNumbers); ++WinIndex)
                    {
                        token Number = RequireToken(Tokenizer, Token_Number);
                        WinningNumbers[WinIndex] = Number.Number;
                    }

                    RequireToken(Tokenizer, Token_Pipe);

                    u32 Points = 0;
                    u32 CopyIndex = CardIndex;
                    u32 Numbers[25];
                    for (u32 NumberIndex = 0; NumberIndex < ArrayCount(Numbers); ++NumberIndex)
                    {
                        token Number = RequireToken(Tokenizer, Token_Number);
                        Numbers[NumberIndex] = Number.Number;
                    }

                    for (u32 NumberIndex = 0; NumberIndex < ArrayCount(Numbers); ++NumberIndex)
                    {
                        for (u32 WinIndex = 0; WinIndex < ArrayCount(WinningNumbers); ++WinIndex)
                        {
                            u32 Result = WinningNumbers[WinIndex];
                            u32 Number = Numbers[NumberIndex];
                            if (Number == Result)
                            {
                                Copies[++CopyIndex] += CardCopies;
                                if (Points == 0)
                                {
                                    Points = 1;
                                }
                                else
                                {
                                    Points *= 2;
                                }
                            }
                        }
                    }

                    SumPoints += Points;

                    token Delimiter = PeekToken(Tokenizer);
                    if (Delimiter.Type == Token_EndOfLine || Delimiter.Type == Token_Identifier || Delimiter.Type ==
                        Token_EndOfStream)
                    {
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }

        u32 TotalScratchcards = 0;
        for (u32 Index = 1; Index <= TotalCards; ++Index)
        {
            TotalScratchcards += Copies[Index];
        }

        printf("Scratchcard points: %u\n", SumPoints);
        printf("Total scratchcards: %u\n", TotalScratchcards);
    }

    return 0;
}

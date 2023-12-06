// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "common/common.h"
#include "common/tokenizer.h"

struct race
{
    u64 Time;
    u64 RecordDist;
};

u64 CalculateWaysToBeatRace(race* Race)
{
    // d = record dist, t = time limit, x = hold time
    // d = xt - x^2
    // trusty quadratic formula solve for x
    s64 A = -1;
    s64 B = (s64)Race->Time;
    s64 C = -(s64)Race->RecordDist;

    f64 Left = (f64)B / 2.0f;
    f64 Right = sqrt(B*B - 4*A*C) / 2.0f;

    // If we land on an exact number we need to correct for ceil/floor
    f64 TestMin = Left - Right;
    if (floor(TestMin) == TestMin)
    {
        TestMin += 0.5f;
    }
    f64 TestMax = Left + Right;
    if (floor(TestMax) == TestMax)
    {
        TestMax -= 0.5f;
    }

    u64 Min = (u64)ceil(TestMin);
    u64 Max = (u64)floor(TestMax);
    u64 WaysToBeat = Max - Min + 1;
    return WaysToBeat;
}

void AppendDecimalDigits(u64* Target, string Text)
{
    while (IsNumber((char)*Text.Data) && Text.Count > 0)
    {
        s32 Digit = *Text.Data - '0';
        *Target = 10 * (*Target) + Digit;
        Advance(&Text, 1);
    }
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile("inputs/daysix.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daysix.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        race Races[4];
        race ActualRace = {};

        u32 RaceCount = 0;
        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                RequireIdentifier(Tokenizer, "Time");
                RequireToken(Tokenizer, Token_Colon);
                token Token = PeekToken(Tokenizer);
                u32 RaceIndex = 0;
                while (Token.Type == Token_Number)
                {
                    Token = GetToken(Tokenizer);
                    Races[RaceIndex++].Time = Token.Number;
                    AppendDecimalDigits(&ActualRace.Time, Token.Text);
                    Token = PeekToken(Tokenizer);
                }
                RaceCount = RaceIndex;

                RequireIdentifier(Tokenizer, "Distance");
                RequireToken(Tokenizer, Token_Colon);
                Token = PeekToken(Tokenizer);
                RaceIndex = 0;
                while (Token.Type == Token_Number)
                {
                    Token = GetToken(Tokenizer);
                    Races[RaceIndex++].RecordDist = Token.Number;
                    AppendDecimalDigits(&ActualRace.RecordDist, Token.Text);
                    Token = PeekToken(Tokenizer);
                }
            }
            else
            {
                break;
            }
        }

        u64 TotalWaysToBeat = 1;
        for (u32 RaceIndex = 0; RaceIndex < RaceCount; ++RaceIndex)
        {
            race* Race = Races + RaceIndex;
            TotalWaysToBeat *= CalculateWaysToBeatRace(Race);
        }

        printf("There are %u ways to beat the records\n", TotalWaysToBeat);
        u64 TotalWayToBeatActual = CalculateWaysToBeatRace(&ActualRace);
        printf("There are really %u ways to beat the records", TotalWayToBeatActual);
    }

    return 0;
}

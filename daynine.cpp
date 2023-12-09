// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "common/common.h"
#include "common/tokenizer.h"

struct sequence
{
    s64 Values[32][32];
    u32 Levels;
    u32 Len;
};

void GenerateLevelsOfSequence(sequence* Sequence)
{
    b32 IsAllZero = false;
    u32 LevelLen = Sequence->Len;
    while (!IsAllZero)
    {
        s64* Level = Sequence->Values[Sequence->Levels - 1];
        s64 LastValue = Level[0];
        b32 ContainedNonZero = false;
        for (u32 ValueIndex = 1; ValueIndex < LevelLen; ++ValueIndex)
        {
            s64 Value = Level[ValueIndex];
            s64 Diff = Value - LastValue;
            Sequence->Values[Sequence->Levels][ValueIndex - 1] = Diff;
            LastValue = Value;
            ContainedNonZero = Diff != 0 || ContainedNonZero;
        }
        IsAllZero = !ContainedNonZero;

        ++Sequence->Levels;
        --LevelLen;
    }
}

void PrintSequenceMap(sequence* Sequence)
{
    u32 LevelLen = Sequence->Len + 1;
    for (u32 LevelIndex = 0; LevelIndex < Sequence->Levels; ++LevelIndex)
    {
        printf("Level %u:", LevelIndex);
        for (u32 ValueIndex = 0; ValueIndex < LevelLen; ++ValueIndex)
        {
            s64 Value = Sequence->Values[LevelIndex][ValueIndex];
            printf(" %lld", Value);
        }
        printf("\n");
        --LevelLen;
    }
}

s64 SolveNextInSequence(sequence* Sequence)
{
    u32 LevelLen = Sequence->Len - Sequence->Levels + 1;
    s64 Result = 0;
    for (s32 Level = (s32)(Sequence->Levels - 1); Level > 0; --Level)
    {
        s64 Increment = Sequence->Values[Level][LevelLen];
        s64 Basis = Sequence->Values[Level - 1][LevelLen++];
        Sequence->Values[Level - 1][LevelLen] = Basis + Increment;
    }
    Result = Sequence->Values[0][LevelLen];
    return Result;
}

s64 SolvePrevInSequence(sequence* Sequence)
{
    s64 Result = Sequence->Values[Sequence->Levels - 1][0];
    for (s32 Level = (s32)(Sequence->Levels - 2); Level >= 0; --Level)
    {
        s64 Increment = Result;
        s64 Basis = Sequence->Values[Level][0];
        Result = Basis - Increment;
    }

    return Result;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/daynine.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daynine.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        u32 SequenceCount = 0;
        sequence* NextSequences = (sequence*)calloc(256, sizeof(sequence));
        memset(NextSequences, 0, sizeof(sequence)*256);

        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                sequence* NextSequence = NextSequences + SequenceCount;
                u32 ValueIndex = 0;
                token Number = PeekToken(Tokenizer);
                while (Number.Type != Token_EndOfLine && Number.Type != Token_EndOfStream)
                {
                    Number = GetToken(Tokenizer);
                    NextSequence->Values[0][ValueIndex++] = Number.Number;
                    Number = PeekToken(Tokenizer, false);
                }
                NextSequence->Levels = 1;
                NextSequence->Len = ValueIndex;
                ++SequenceCount;
            }
            else
            {
                break;
            }
        }

        s64 SumOfNextValues = 0;
        s64 SumOfPrevValues = 0;
        for (u32 SequenceIndex = 0; SequenceIndex < SequenceCount; ++SequenceIndex)
        {
            sequence* Sequence = NextSequences + SequenceIndex;
            GenerateLevelsOfSequence(Sequence);
            SumOfNextValues += SolveNextInSequence(Sequence);
            SumOfPrevValues += SolvePrevInSequence(Sequence);

            printf("Sequence %u:\n", SequenceIndex);
            PrintSequenceMap(Sequence);
        }

        printf("Sum of next values is %lld\n", SumOfNextValues);
        printf("Sum of prev values is %lld\n", SumOfPrevValues);
    }

    return 0;
}

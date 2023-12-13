// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string.h>

#include "common/common.h"
#include "common/tokenizer.h"

//
// IMPORTANT: This doesn't work, may or may not get time to retry so don't bother looking at for inspiration unless you want to head down a most likely bad path
//

enum spring_type
{
    Unknown,
    Operational,
    Damaged,
};

struct line
{
    u8 RunCount;
    u8 Runs[8];
    u8 CombinedRunLength;
    u8 LongestRun;
    string Input;
};

struct run
{
    u8 Min;
    u8 Max;
};

u32 CalculatePermutations(line* Line, spring_type* Springs, u8 RunIndex, u8 MinPos)
{
    u32 TotalPermutations = 0;

    u8 Run = Line->Runs[RunIndex];
    u8 MaxPossible = (u8)Line->Input.Count;

    u8 Min = MinPos;
    u8 Max = MaxPossible;
    b32 ScanForMin = true;
    u8 Len = 0;
    u32 PossibleLocations = 0;
    u8 Damage[8] = {};
    u32 DamageCount = 0;
    u32 Permutations = 0;
    for (u32 Test = Min; Test < MaxPossible; ++Test)
    {
        spring_type Spring = Springs[Test];
        if (Spring != Operational && DamageCount < Run && !(DamageCount > 0 && Damage[0] + Run < Test))
        {
            if (ScanForMin)
            {
                Min = Test;
                ScanForMin = false;
            }
            if (Spring == Damaged)
            {
                Damage[DamageCount++] = Test;
            }
            ++PossibleLocations;
        }
        else
        {
            if (!ScanForMin)
            {
                Len = Test - Min;
                // Assert(Len >= Run);
                Max = Test;
                break;
            }
        }
    }

    if (DamageCount > 0)
    {
        for (u32 DamageIndex = 0; DamageIndex < DamageCount; ++DamageIndex)
        {
            u8 DamageLocation = Damage[DamageIndex];
            if (Max - DamageLocation == 1)
            {
                Max += 1;
            }
        }

        if (DamageCount == Run)
        {
            PossibleLocations = 1;
        }
        else
        {
            // How do we establish the permutations of the stride across a broken spring if we have wildcards either side
            // And use this to adjust our Max value so that we either push or pull the next Run based on our permutations
            // ??#?? 2           .##.. ..##.
            // ??#?? 3           ###.. .###. ..###
            // ??#??? 4 could be ####.. .####. ..####
            // ??#??? 5 could be #####. .#####

            u8 DamageBase = Damage[0];
            u8 MaximumDamage = 0;
            for (u8 DamageIndex = 1; DamageIndex < DamageCount; ++DamageIndex)
            {
                u8 ThisDamage = Damage[DamageIndex] + 1;
                if (MaximumDamage < ThisDamage)
                {
                    MaximumDamage = ThisDamage;
                }
                if (ThisDamage - DamageBase >= Run)
                {
                    break;
                }
            }
            u8 breakHere = 0;
        }
    }

    if (Len == Run)
    {
        Permutations = 1;
    }
    else
    {
        if (Run > 1)
        {
            Permutations = (u8)ceil((f32)(PossibleLocations + 1) / (f32)Run);
        }
        else
        {
            Permutations = PossibleLocations;
        }
    }

    for (u32 PermutationIndex = 0; PermutationIndex < Permutations; ++PermutationIndex)
    {
        if ((RunIndex + 1) < Line->RunCount)
        {
            TotalPermutations += CalculatePermutations(Line, Springs, RunIndex + 1, Max + PermutationIndex);
        }
        else
        {
            return Permutations;
        }
    }

    return TotalPermutations;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/daytwelve.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daytwelve.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        u32 LineCount = 0;
        line Lines[1024] = {};

        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            string LineText = Tokenizer->Input;
            if (LineStart.Type != Token_EndOfStream)
            {
                line* Line = Lines + LineCount++;
                token Symbol = GetTokenRaw(Tokenizer);
                while (Symbol.Type != Token_EndOfLine && Symbol.Type != Token_EndOfStream)
                {
                    if (Symbol.Type == Token_Spacing)
                    {
                        Line->Input.Count = Symbol.Text.Data - LineText.Data;
                        Line->Input.Data = LineText.Data;
                    }
                    if (Symbol.Type == Token_Number)
                    {
                        u8 Run = (u8)Symbol.Number;
                        Line->Runs[Line->RunCount++] = Run;
                        Line->CombinedRunLength += Run;
                        if (Run > Line->LongestRun)
                        {
                            Line->LongestRun = Run;
                        }
                    }
                    Symbol = GetTokenRaw(Tokenizer);
                }
            }
            else
            {
                break;
            }
        }

        spring_type Springs[32];
        run Runs[8];
        u32 TotalPossibleArrangements = 0;
        for (u32 LineIndex = 0; LineIndex < LineCount; ++LineIndex)
        {
            // Clear scratch memory every iteration
            memset(Springs, 0, 32 * sizeof(spring_type));
            memset(Runs, 0, 8 * sizeof(run));

            line* Line = Lines + LineIndex;
            u32 LineLength = (u32)Line->Input.Count;
            for (u32 Index = 0; Index < LineLength; ++Index)
            {
                char C = (char)Line->Input.Data[Index];
                switch (C)
                {
                    // @formatter:off
                    case '?': {Springs[Index] = Unknown;} break;
                    case '.': {Springs[Index] = Operational;} break;
                    case '#': {Springs[Index] = Damaged;} break;
                    default: {Assert(!"Unknown spring type");}
                    // @formatter:on
                }
            }

            // Special case: there is only one possible arrangement due to the input length matching that of the runs with a single space between each run
            u32 TotalPossibleLocations = 1;
            if ((Line->CombinedRunLength + Line->RunCount - 1) == LineLength)
            {
                TotalPossibleArrangements += TotalPossibleLocations;
            }
            else
            {
                TotalPossibleArrangements = CalculatePermutations(Line, Springs, 0, 0);
            }
            printf("Line %u total arrangements: %u\n", LineIndex, TotalPossibleArrangements);
        }

        for (u32 LineIndex = 0; LineIndex < LineCount; ++LineIndex)
        {
            line* Line = Lines + LineIndex;
            printf("Line: %.*s\n", (s32)Line->Input.Count, (char *)Line->Input.Data);
            printf("Runs: %u\n\t", Line->RunCount);
            for (u32 RunIndex = 0; RunIndex < Line->RunCount; ++RunIndex)
            {
                u8 RunLength = Line->Runs[RunIndex];
                printf(" %u", RunLength);
            }
            printf("\n");
        }
    }

    return 0;
}

// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>

#include "common/common.h"
#include "common/tokenizer.h"

struct pattern
{
    u32 HorizontalLine[32];
    u32 VerticalLine[32];
    u32 LineCount;
    u32 LineWidth;
    b32 WasHorizontal;
    u32 OriginalResult;
};

u32 CountLinesUntilMirrorOrZero(u32* Line, u32 LineCount, u32 OriginalResult = 0)
{
    s32 A = 0;
    s32 B = 1;

    u32 PossibleA[8];
    u32 PossibleCount = 0;
    while (A < LineCount && B < LineCount)
    {
        u32 LineA = Line[A];
        u32 LineB = Line[B];

        if (LineA == LineB)
        {
            PossibleA[PossibleCount++] = A;
        }
        ++A;
        ++B;
    }

    u32 LinesUntil = 0;

    b32 MatchFound = false;
    for (u32 AIndex = 0; AIndex < PossibleCount; ++AIndex)
    {
        A = (s32)PossibleA[AIndex];
        B = A + 1;
        LinesUntil = B;
        MatchFound = true;
        while (MatchFound && A >= 0 && B < LineCount)
        {
            u32 LineA = Line[A];
            u32 LineB = Line[B];

            MatchFound = LineA == LineB;
            --A;
            ++B;
        }

        // Don't break on the same match in part 2 otherwise we end up not fully testing our modified row/column
        if (MatchFound && OriginalResult != LinesUntil)
        {
            break;
        }
    }

    if (!MatchFound)
    {
        LinesUntil = 0;
    }

    return LinesUntil;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/daythirteen.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daythirteen.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        pattern Patterns[1024] = {};
        u32 PatternCount = 0;

        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                pattern* Pattern = Patterns + PatternCount++;

                token Symbol = PeekToken(Tokenizer);
                u32 EndOfLineCount = 0;
                u32 LineIndex = 0;
                while (EndOfLineCount < 2)
                {
                    u32 ColumnIndex = 0;
                    Symbol = GetTokenRaw(Tokenizer);
                    while (Symbol.Type != Token_EndOfLine && Symbol.Type != Token_EndOfStream)
                    {
                        if (EndOfLineCount > 0)
                        {
                            EndOfLineCount = 0;
                            ++LineIndex;
                        }

                        Pattern->HorizontalLine[LineIndex] <<= 1;
                        Pattern->HorizontalLine[LineIndex] |= Symbol.Type == Token_Pound;

                        Pattern->VerticalLine[ColumnIndex] <<= 1;
                        Pattern->VerticalLine[ColumnIndex] |= Symbol.Type == Token_Pound;
                        ++ColumnIndex;

                        Pattern->LineWidth = ColumnIndex;
                        Symbol = GetTokenRaw(Tokenizer);
                    }
                    if (Symbol.Type == Token_EndOfStream)
                    {
                        break;
                    }
                    else
                    {
                        ++EndOfLineCount;
                    }
                }
                Pattern->LineCount = LineIndex + 1;
            }
            else
            {
                break;
            }
        }

        // Part 1
        u32 SumPatterns = 0;
        for (u32 PatternIndex = 0; PatternIndex < PatternCount; ++PatternIndex)
        {
            pattern* Pattern = Patterns + PatternIndex;

            printf("Pattern %u\n", PatternIndex);

            u32 RowsAbove = CountLinesUntilMirrorOrZero(Pattern->HorizontalLine, Pattern->LineCount);
            u32 ColumnsBefore = CountLinesUntilMirrorOrZero(Pattern->VerticalLine, Pattern->LineWidth);

            if (RowsAbove > 0)
            {
                printf("There were %u Rows Above the mirror line\n", RowsAbove);
                SumPatterns += 100 * RowsAbove;
                Pattern->OriginalResult = RowsAbove;
                Pattern->WasHorizontal = true;
            }
            if (ColumnsBefore > 0)
            {
                printf("There were %u Columns Before the mirror line\n", ColumnsBefore);
                SumPatterns += ColumnsBefore;
                Pattern->OriginalResult = ColumnsBefore;
                Pattern->WasHorizontal = false;
            }
        }

        printf("\n");
        printf("The sum for Part 1 is %u\n", SumPatterns);
        printf("\n");

        u32 SumPatternsPart2 = 0;
        for (u32 PatternIndex = 0; PatternIndex < PatternCount; ++PatternIndex)
        {
            pattern* Pattern = Patterns + PatternIndex;

            printf("Pattern %u\n", PatternIndex);

            u32 Permutations = Pattern->LineCount * Pattern->LineWidth;
            u32 VerticalBit = Pattern->LineCount;
            for (u32 BitIndex = 0; BitIndex < Permutations; ++BitIndex)
            {
                pattern Temp = *Pattern;
                u32 Row = BitIndex / Pattern->LineWidth;
                u32 Column = BitIndex % Pattern->LineWidth;
                // Can this be done better somehow?
                if (Column == 0)
                {
                    --VerticalBit;
                }

                u32 HorizontalBit = Pattern->LineWidth - (BitIndex % Pattern->LineWidth) - 1;
                // Xor to mutate our pattern in the same spot for both HorizontalLine and VerticalLine
                Temp.HorizontalLine[Row] ^= (1 << HorizontalBit);
                Temp.VerticalLine[Column] ^= (1 << VerticalBit);

                u32 RowsAbove = CountLinesUntilMirrorOrZero(Temp.HorizontalLine, Pattern->LineCount, Pattern->WasHorizontal ? Pattern->OriginalResult : 0);
                u32 ColumnsBefore = CountLinesUntilMirrorOrZero(Temp.VerticalLine, Pattern->LineWidth, !Pattern->WasHorizontal ? Pattern->OriginalResult : 0);

                if (RowsAbove > 0 && (RowsAbove != Pattern->OriginalResult || !Pattern->WasHorizontal))
                {
                    printf("There were %u Rows Above the mirror line\n", RowsAbove);
                    SumPatternsPart2 += 100 * RowsAbove;
                    break;
                }
                else if (ColumnsBefore > 0 && (ColumnsBefore != Pattern->OriginalResult || Pattern->WasHorizontal))
                {
                    printf("There were %u Columns Before the mirror line\n", ColumnsBefore);
                    SumPatternsPart2 += ColumnsBefore;
                    break;
                }
            }
        }

        printf("\n");
        printf("The sum for Part 2 is %u", SumPatternsPart2);
    }

    return 0;
}

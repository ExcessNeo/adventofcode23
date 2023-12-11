// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "common/common.h"
#include "common/tokenizer.h"

struct galaxy
{
    u16 Id;
    u64 X;
    u64 Y;
    b32 Pushed;
};

struct galaxy_pair
{
    galaxy A;
    galaxy B;
};

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/dayeleven.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/dayeleven.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        galaxy Galaxies[512] = {};
        u32 GalaxyCount = 0;

        u8 GalaxiesInColumn[140] = {};
        u32 ColumnCount = 0;

        u32 YIndex = 1;

        // There's some alteration based on the expansion rate being above 1 that is used later on to not expand too far
#if 0
        u32 ExpansionRate = 1;
#else
        u32 ExpansionRate = 1000000;
#endif
        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                u8 GalaxiesInRow = 0;
                u16 XIndex = 1;
                token Symbol = PeekToken(Tokenizer);
                while (Symbol.Type != Token_EndOfLine && Symbol.Type != Token_EndOfStream)
                {
                    Symbol = GetToken(Tokenizer);
                    if (Symbol.Type == Token_Pound)
                    {
                        galaxy* Galaxy = Galaxies + GalaxyCount;
                        Galaxy->Id = GalaxyCount++;
                        Galaxy->X = XIndex;
                        Galaxy->Y = YIndex;
                        ++GalaxiesInColumn[XIndex];
                        ++GalaxiesInRow;
                    }
                    ++XIndex;
                    Symbol = PeekToken(Tokenizer, false);
                }
                ColumnCount = XIndex;

                if (GalaxiesInRow == 0)
                {
                    // Expand the empty row
                    if (ExpansionRate == 1)
                    {
                        ++YIndex;
                    }
                    YIndex += ExpansionRate;
                }
                else
                {
                    ++YIndex;
                }
            }
            else
            {
                break;
            }
        }

        // Expand empty columns
        u32 OffsetX = 0;
        u32 MaxX = 0;
        for (u32 ColIndex = 1; ColIndex < ColumnCount; ++ColIndex)
        {
            u8 Count = GalaxiesInColumn[ColIndex];
            if (Count == 0)
            {
                if (ExpansionRate == 1)
                {
                    ++OffsetX;
                }
                else
                {
                    OffsetX += ExpansionRate - 1;
                }
            }
            for (u32 GalaxyIndex = 0; GalaxyIndex < GalaxyCount; ++GalaxyIndex)
            {
                galaxy* Galaxy = Galaxies + GalaxyIndex;
                if (OffsetX > 0 && Galaxy->X == ColIndex && !Galaxy->Pushed)
                {
                    Galaxy->X += OffsetX;
                    Galaxy->Pushed = true;
                    if (Galaxy->X > MaxX)
                    {
                        MaxX = Galaxy->X;
                    }
                }
            }
        }

        // Don't try to print beyond small expansion sizes otherwise it will take a very long time for the program to execute
#if 0
        u32 CurrentX = 1;
        u32 CurrentY = 1;
        for (u32 GalaxyIndex = 0; GalaxyIndex < GalaxyCount; ++GalaxyIndex)
        {
            galaxy* Galaxy = Galaxies + GalaxyIndex;
            for (; CurrentY < Galaxy->Y; ++CurrentY)
            {
                for (u32 Pad = CurrentX; Pad < MaxX; ++Pad)
                {
                    printf(".");
                }
                CurrentX = 1;
                printf("\n");
            }
            u32 DeltaX = Galaxy->X - CurrentX;
            for (u32 Pad = 0; Pad < DeltaX; ++Pad)
            {
                printf(".");
            }
            printf("%u", Galaxy->Id);
            CurrentX = Galaxy->X + 1;
        }
#endif

        printf("\n");

        u32 PairCount = (GalaxyCount * (GalaxyCount - 1)) / 2;
        galaxy_pair* Pairs = (galaxy_pair*)malloc(PairCount * sizeof(galaxy_pair));
        u32 Index = 0;
        for (u32 GalaxyIndex = 0; GalaxyIndex < GalaxyCount; ++GalaxyIndex)
        {
            galaxy* A = Galaxies + GalaxyIndex;
            for (u32 PairIndex = GalaxyIndex + 1; PairIndex < GalaxyCount; ++PairIndex)
            {
                galaxy* B = Galaxies + PairIndex;
                galaxy_pair* Pair = Pairs + Index++;
                Pair->A = *A;
                Pair->B = *B;
            }
        }

        printf("Found %u pairs\n", PairCount);

        u64 SumPath = 0;
        for (u32 PairIndex = 0; PairIndex < PairCount; ++PairIndex)
        {
            galaxy_pair* Pair = Pairs + PairIndex;
            s64 DeltaX = Pair->B.X - Pair->A.X;
            s64 DeltaY = Pair->B.Y - Pair->A.Y;
            printf("A (%llu,%llu) -> B (%llu, %llu): (%lld, %lld)\n", Pair->A.X, Pair->A.Y, Pair->B.X, Pair->B.Y, DeltaX, DeltaY);
            // Sum the Manhattan Distance
            SumPath += abs(DeltaX) + abs(DeltaY);
        }

        printf("Sum of paths: %llu\n", SumPath);
    }

    return 0;
}

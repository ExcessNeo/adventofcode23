// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>

#include "common/common.h"
#include "common/tokenizer.h"

#define TIME 0
#if TIME
#include <Windows.h>
#endif

enum grid_occupier:u8
{
    Empty,
    Round,
    Cube
};

enum tilt_orientation
{
    North,
    West,
    South,
    East,
};

struct tuple
{
    u64 HashValue;
    u64 LastIndex;
    u32 Load;
};

void TiltLever(grid_occupier* Grid, u32 GridWidth, u32 GridHeight, tilt_orientation Orientation)
{
    s32 Stride = 0;
    u32 XStart = 0;
    u32 YStart = 0;
    s32 XLimit = GridWidth;
    s32 YLimit = GridHeight;
    s32 XOffset = 1;
    s32 YOffset = 1;
    b32 RowLock = false;
    switch (Orientation)
    {
        case North:
        {
            Stride = -GridWidth;
            YStart = 1;
        }
        break;
        case West:
        {
            Stride = -1;
            XStart = 1;
            RowLock = true;
        }
        break;
        case South:
        {
            Stride = GridWidth;
            YStart = GridHeight - 2;
            YLimit = -1;
            YOffset = -1;
        }
        break;
        case East:
        {
            Stride = 1;
            XStart = GridWidth - 2;
            XLimit = -1;
            XOffset = -1;
            RowLock = true;
        }
        break;
    }

    grid_occupier* GridMax = Grid + GridWidth * GridHeight;

    for (s32 Y = YStart; Y != YLimit; Y += YOffset)
    {
        for (s32 X = XStart; X != XLimit; X += XOffset)
        {
            grid_occupier* Occupier = &Grid[(Y * GridWidth) + X];
            if (*Occupier == Round)
            {
                grid_occupier* AboveOccupier = Occupier + Stride;

                grid_occupier* Min = Grid;
                grid_occupier* Max = GridMax;
                if (RowLock)
                {
                    Min = Grid + (Y * GridWidth);
                    Max = Grid + ((Y + 1) * GridWidth);
                }

                while (AboveOccupier >= Min && AboveOccupier < Max && *AboveOccupier == Empty)
                {
                    *AboveOccupier = Round;
                    *Occupier = Empty;
                    Occupier = AboveOccupier;
                    AboveOccupier = Occupier + Stride;
                }
            }
        }
    }
}

u32 ComputeLoad(grid_occupier* Grid, u32 GridWidth, u32 GridHeight)
{
    u32 Result = 0;
    for (u32 Y = 0; Y < GridHeight; ++Y)
    {
        u32 Coefficient = GridHeight - Y;
        for (u32 X = 0; X < GridWidth; ++X)
        {
            grid_occupier Occupier = Grid[(Y * GridWidth) + X];
            u32 Weight = Occupier == Round ? 1 : 0;
            Result += Weight * Coefficient;
        }
    }
    return Result;
}

void DisplayGrid(grid_occupier* Grid, u32 GridWidth, u32 GridHeight)
{
    printf("Grid: \n");
    for (u32 Y = 0; Y < GridHeight; ++Y)
    {
        for (u32 X = 0; X < GridWidth; ++X)
        {
            grid_occupier Occupier = Grid[(Y * GridWidth) + X];
            if (Occupier == Empty)
            {
                printf(".");
            }
            else if (Occupier == Round)
            {
                printf("O");
            }
            else if (Occupier == Cube)
            {
                printf("#");
            }
        }
        printf("\n");
    }
    printf("\n");
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/dayfourteen.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        grid_occupier Grid[16384] = {};
        u32 GridWidth = 0;
        u32 GridHeight = 0;

        grid_occupier* Occupier = Grid;
        while (FileBuffer.Count)
        {
            GridWidth = 0;
            char C = *(char *)Advance(&FileBuffer, 1);
            while (!IsEndOfLine(C) && FileBuffer.Count)
            {
                grid_occupier Result;
                switch (C)
                {
                    case 'O':
                    {
                        Result = Round;
                    }
                    break;
                    case '.':
                    {
                        Result = Empty;
                    }
                    break;
                    case '#':
                    {
                        Result = Cube;
                    }
                    break;
                    default:
                    {
                        break;
                    }
                }

                *Occupier++ = Result;
                if (FileBuffer.Count)
                {
                    C = *(char *)Advance(&FileBuffer, 1);
                }
                ++GridWidth;
            }
            if (IsEndOfLine(*FileBuffer.Data))
            {
                ++GridHeight;
            }
        }

        GridWidth += 1;
        GridHeight += 1;

#if 1
        // Part 1
        DisplayGrid(Grid, GridWidth, GridHeight);
        TiltLever(Grid, GridWidth, GridHeight, North);
        printf("After tilt\n");
        DisplayGrid(Grid, GridWidth, GridHeight);
#else
        // Part 2

        u64 CycleCount = 1000000000;
        // TODO: Figure out a reasonable hash strategy, and try to identify a cycle to solve part 2
#if TIME
        LARGE_INTEGER Freq;
        QueryPerformanceFrequency(&Freq);

        LARGE_INTEGER LastTime;
        QueryPerformanceCounter(&LastTime);
        f64 InvCycleCount = 1.0 / (f64)CycleCount;
        u32 ChunkSize = 10000;
        u32 TotalChunks = CycleCount / ChunkSize;
#endif
        for (u64 CycleIndex = 0; CycleIndex < CycleCount; ++CycleIndex)
        {
            // printf("Spin cycle %llu\n", CycleIndex);
            TiltLever(Grid, GridWidth, GridHeight, North);
            // printf("After North\n");
            // DisplayGrid(Grid, GridWidth, GridHeight);

            TiltLever(Grid, GridWidth, GridHeight, West);
            // printf("After West\n");
            // DisplayGrid(Grid, GridWidth, GridHeight);

            TiltLever(Grid, GridWidth, GridHeight, South);
            // printf("After South\n");
            // DisplayGrid(Grid, GridWidth, GridHeight);

            TiltLever(Grid, GridWidth, GridHeight, East);
            // printf("After East\n");
            // DisplayGrid(Grid, GridWidth, GridHeight);

#if TIME
            if (CycleIndex % ChunkSize == 0)
            {
                u32 CurrentChunk = CycleIndex / TotalChunks;
                u32 ChunksLeft = TotalChunks - CurrentChunk;
                LARGE_INTEGER CurrentTime;
                QueryPerformanceCounter(&CurrentTime);
                double ElapsedTime = (double)(CurrentTime.QuadPart - LastTime.QuadPart) / (double)Freq.QuadPart;
                double ApproximateFinish = ElapsedTime * (double)ChunksLeft;

                printf("There are %f%% cycles completed and the last %u took %f seconds to execute with a projected finish in %f seconds\n", CycleIndex * InvCycleCount * 100, ChunkSize, ElapsedTime, ApproximateFinish);
                LastTime = CurrentTime;
            }
#endif
        }
#endif

        u32 Load = ComputeLoad(Grid, GridWidth, GridHeight);
        printf("The total load is: %u\n", Load);
    }

    return 0;
}

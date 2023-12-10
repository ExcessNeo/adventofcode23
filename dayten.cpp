// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "common/common.h"

#define MAZE_DIM 140

enum dir
{
    West = 0x1,
    South = 0x2,
    East = 0x4,
    North = 0x8,
};

dir Opposite(dir A)
{
    if (A == West)
    {
        return East;
    }
    else if (A == South)
    {
        return North;
    }
    else if (A == East)
    {
        return West;
    }
    else if (A == North)
    {
        return South;
    }
    else
    {
        Assert(!"Invalid input");
    }
}

enum piece_type
{
    Ground = 0x0,
    Vertical = North | South,
    Horizontal = East | West,
    NorthEastBend = North | East,
    NorthWestBend = North | West,
    SouthWestBend = South | West,
    SouthEastBend = South | East,
    Start = North | East | South | West,
    Inside = 16,
};

struct maze_piece
{
    piece_type Type;
    maze_piece* Forward;
    maze_piece* Backward;
};

b32 IsLoopPart(maze_piece* Piece)
{
    b32 Result = Piece->Type != Ground && (Piece->Backward != nullptr || Piece->Forward != nullptr);
    return Result;
}

u32 GetDistanceOfFurthestPoint(maze_piece* Maze, u32 XStart, u32 XCount, u32 YStart, u32 YCount)
{
    maze_piece* Start = &Maze[(YStart * MAZE_DIM) + XStart];
    maze_piece* CurrentPosition = nullptr;

    // Test the 4 cardinal directions to see which way the loop could travel
    // This worked for my input but not all test cases so most likely something funky with this block

    dir ToDir;
    u32 Part1 = 0;
    u32 Part2 = 0;
    if (XStart > 0)
    {
        maze_piece* TestPosition = Start - 1;
        if (TestPosition->Type == Horizontal || TestPosition->Type == NorthEastBend || TestPosition->Type ==
            SouthEastBend)
        {
            CurrentPosition = TestPosition;
            Part1 = West;
            ToDir = West;
        }
    }
    if (XStart < XCount)
    {
        maze_piece* TestPosition = Start + 1;
        if (TestPosition->Type == Horizontal || TestPosition->Type == NorthWestBend || TestPosition->Type ==
            SouthWestBend)
        {
            CurrentPosition = TestPosition;
            if (Part1 != 0)
            {
                Part2 = East;
            }
            else
            {
                Part1 = East;
            }
            ToDir = East;
        }
    }
    if (YStart > 0)
    {
        maze_piece* TestPosition = Start - MAZE_DIM;
        if (TestPosition->Type == Vertical || TestPosition->Type == SouthEastBend || TestPosition->Type ==
            SouthWestBend)
        {
            CurrentPosition = TestPosition;
            if (Part1 != 0)
            {
                Part2 = North;
            }
            else
            {
                Part1 = North;
            }
            ToDir = North;
        }
    }
    if (YStart < YCount)
    {
        maze_piece* TestPosition = Start + MAZE_DIM;
        if (TestPosition->Type == Vertical || TestPosition->Type == NorthEastBend || TestPosition->Type ==
            NorthWestBend)
        {
            CurrentPosition = TestPosition;
            if (Part1 != 0)
            {
                Part2 = South;
            }
            else
            {
                Part1 = South;
            }
            ToDir = South;
        }
    }
    // Convert Start to be an actual pipe rather than a wildcard based on the test pieces above
    Start->Type = (piece_type)(Part1 | Part2);

    Assert(CurrentPosition);
    u32 DistToStart = 1;
    while (CurrentPosition != Start)
    {
        s32 Offset = 0;
        switch (ToDir)
        {
            case North:
            {
                Offset = -MAZE_DIM;
            }
            break;

            case East:
            {
                Offset = 1;
            }
            break;

            case South:
            {
                Offset = MAZE_DIM;
            }
            break;

            case West:
            {
                Offset = -1;
            }
            break;
        }

        maze_piece* Next = CurrentPosition + Offset;
        dir FromDir = Opposite(ToDir);
        ToDir = (dir)(Next->Type & ~FromDir);

        CurrentPosition->Forward = Next;
        Next->Backward = CurrentPosition;
        CurrentPosition = Next;
        ++DistToStart;
    }

    u32 Result = DistToStart / 2;
    return Result;
}

u32 FindNestAreas(maze_piece* Maze)
{
    u32 InsideLoopCount = 0;
    for (u32 Y = 0; Y < MAZE_DIM; ++Y)
    {
        b32 InRegion = false;
        maze_piece* LastCorner = nullptr;
        for (u32 X = 0; X < MAZE_DIM; ++X)
        {
            maze_piece* Piece = &Maze[(Y * MAZE_DIM) + X];

            // Clear non piece parts to ground to avoid scanline picking up non loop parts accidentally
            if (!IsLoopPart(Piece))
            {
                Piece->Type = Ground;
            }

            switch (Piece->Type)
            {
                case Vertical:
                {
                    InRegion = !InRegion;
                }
                break;
                case NorthEastBend:
                {
                    LastCorner = Piece;
                }
                break;
                case NorthWestBend:
                {
                    if (LastCorner && LastCorner->Type == NorthEastBend)
                    {
                        // Didn't change region was a U shape
                        LastCorner = nullptr;
                    }
                    else
                    {
                        InRegion = !InRegion;
                    }
                }
                break;
                case SouthWestBend:
                {
                    if (LastCorner && LastCorner->Type == SouthEastBend)
                    {
                        // Didn't change region was a U shape
                        LastCorner = nullptr;
                    }
                    else
                    {
                        InRegion = !InRegion;
                    }
                }
                break;
                case SouthEastBend:
                {
                    LastCorner = Piece;
                }
                break;
                case Ground:
                {
                    if (InRegion)
                    {
                        ++InsideLoopCount;
                        Piece->Type = Inside;
                    }
                }
                break;
                default:
                {
                    // Do nothing
                }
            }
        }
    }

    return InsideLoopCount;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/dayten.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        maze_piece* Maze = (maze_piece *)malloc(MAZE_DIM * MAZE_DIM * sizeof(maze_piece));

        u32 XStart = 0;
        u32 YStart = 0;

        u32 XIndex = 0;
        u32 YIndex = 0;
        u32 XCount = 0;
        u32 YCount = 0;
        char* At = (char *)FileBuffer.Data;
        while (At != (char *)&FileBuffer.Data[FileBuffer.Count])
        {
            maze_piece* Piece = &Maze[(YIndex * MAZE_DIM) + XIndex];
            switch (*At)
            {
                // @formatter:off
                case '|': {Piece->Type = Vertical;} break;
                case '-': {Piece->Type = Horizontal;} break;
                case 'L': {Piece->Type = NorthEastBend;} break;
                case 'J': {Piece->Type = NorthWestBend;} break;
                case '7': {Piece->Type = SouthWestBend;} break;
                case 'F': {Piece->Type = SouthEastBend;} break;
                case 'S':
                {
                    Piece->Type = Start;
                    XStart = XIndex;
                    YStart = YIndex;
                } break;
                default: {Piece->Type = Ground;}
                // @formatter:on
            }
            Piece->Backward = nullptr;
            Piece->Forward = nullptr;
            ++XIndex;
            ++At;
            if (IsEndOfLine(*At))
            {
                ++At;
                // Handle windows end of line
                if (IsEndOfLine(*At))
                {
                    ++At;
                }
                ++YIndex;
                XCount = XIndex;
                XIndex = 0;
            }
        }
        YCount = YIndex;

        u32 Distance = GetDistanceOfFurthestPoint(Maze, XStart, XCount, YStart, YCount);
        u32 InsideCount = FindNestAreas(Maze);

        for (u32 Y = 0; Y < YCount; ++Y)
        {
            for (u32 X = 0; X < XCount; ++X)
            {
                maze_piece* Piece = &Maze[(Y * MAZE_DIM) + X];
                piece_type Value = Piece->Type;
                char out;
                switch (Value)
                {
                    // @formatter:off
                    case Vertical: {out = '|';} break;
                    case Horizontal: {out = '-';} break;
                    case NorthEastBend: {out = 'L';} break;
                    case NorthWestBend: {out = 'J';} break;
                    case SouthWestBend: {out = '7';} break;
                    case SouthEastBend: {out = 'F';} break;
                    case Start: {out = 'S';} break;
                    case Inside: {out = 'I';} break;
                    default: {out = ' ';}
                    // @formatter:on
                }
                printf("%c", out);
            }
            printf("\n");
        }

        printf("Dist to start: %u\n", Distance);
        printf("There are %u tiles enclosed by the loop\n", InsideCount);
    }

    return 0;
}

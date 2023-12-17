// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cmath>
#include <cstdio>

#include "common/common.h"
#include "common/tokenizer.h"
#include "common/debug.h"

enum tile_type
{
    Empty,
    // Forward slash
    ForwardMirror,
    // Backslash
    BackwardMirror,
    HorizontalSplitter,
    VerticalSplitter,
    OutOfBounds,
};

enum dir
{
    West = 0x1,
    South = 0x2,
    East = 0x4,
    North = 0x8,
};

struct tile
{
    tile_type Type;
    b32 Energized;
    b32 Dim;
    u32 HitFromMask;
};

struct v2i
{
    s32 X;
    s32 Y;
};

v2i operator+(v2i A, v2i B)
{
    v2i Result = {};
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return Result;
}

v2i& operator+=(v2i&B, v2i A)
{
    B = B + A;
    return B;
}

v2i operator-(v2i A)
{
    v2i Result = {};
    Result.X = -A.X;
    Result.Y = -A.Y;
    return Result;
}

dir FromV2i(v2i A)
{
    dir Result;
    if (A.X < 0)
    {
        Result = West;
    }
    else if (A.X > 0)
    {
        Result = East;
    }
    else if (A.Y < 0)
    {
        Result = North;
    }
    else if (A.Y > 0)
    {
        Result = South;
    }
    else
    {
        Assert(!"Invalid direction passed");
    }
    return Result;
}

struct beam
{
    v2i Dir;
    v2i Pos;
    v2i Start;
    b32 Terminated;
};

tile_type AdvanceBeam(beam* Beam, tile* Tiles, u32 TileDim)
{
    tile_type Result;
    if (Beam->Pos.X < 0 || Beam->Pos.Y < 0 || Beam->Pos.X >= TileDim || Beam->Pos.Y >= TileDim || Beam->Terminated)
    {
        Result = OutOfBounds;
    }
    else
    {
        u32 BeamPos = (Beam->Pos.Y * TileDim + Beam->Pos.X);
        tile* Tile = Tiles + BeamPos;
        Tile->Energized = true;
        Result = Tile->Type;
        dir BeamDir = FromV2i(Beam->Dir);
        while (!Beam->Terminated && Result == Empty)
        {
            if ((Result == Empty && (BeamDir == West && Beam->Pos.X == 0 ||
                BeamDir == East && Beam->Pos.X == (TileDim - 1) ||
                BeamDir == North && Beam->Pos.Y == 0 ||
                BeamDir == South && Beam->Pos.Y == (TileDim - 1))))
            {
                Result = OutOfBounds;
                Beam->Terminated = true;
            }
            else
            {
                if (Tile->HitFromMask & BeamDir)
                {
                    // We already hit this tile from this direction so terminate the beam
                    Beam->Terminated = true;;
                }
                else
                {
                    Tile->HitFromMask |= BeamDir;
                    Beam->Pos += Beam->Dir;
                    BeamPos = (Beam->Pos.Y * TileDim + Beam->Pos.X);
                    Assert(Beam->Pos.X >= 0 && Beam->Pos.X < TileDim);
                    Assert(Beam->Pos.Y >= 0 && Beam->Pos.Y < TileDim);
                    Tile = Tiles + BeamPos;
                    Tile->Energized = true;
                    Result = Tile->Type;
                }
            }
        }
    }
    return Result;
}

u32 PrintGridAndCalculateEnergized(tile* Tiles, u32 TileDim)
{
    u32 EnergizedCount = 0;
    HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
    DebugClearScreen(Console);
    for (u32 Y = 0; Y < TileDim; ++Y)
    {
        for (u32 X = 0; X < TileDim; ++X)
        {
            tile* Tile = Tiles + (Y * TileDim + X);
            if (Tile->Energized)
            {
                if (Tile->Dim)
                {
                    DebugSetColour(Console, DB_Dim);
                }
                else
                {
                    DebugSetColour(Console, DB_Highlight);
                    Tile->Dim = true;
                }
                printf("#");
                ++EnergizedCount;
            }
            else
            {
                DebugSetColour(Console, DB_Default);
                switch (Tile->Type)
                {
                    case Empty:
                        printf(".");
                        break;
#if 1
                    case ForwardMirror:
                        printf("/");
                        break;
                    case BackwardMirror:
                        printf("\\");
                        break;
                    case HorizontalSplitter:
                        printf("-");
                        break;
                    case VerticalSplitter:
                        printf("|");
                        break;
#endif
                    default:
                    {
                        printf(" ");
                    }
                }
            }
        }
        printf("\n");
    }

    printf("There were %u energized tiles\n", EnergizedCount);
    return EnergizedCount;
}

u32 ProcessGridFromStartingPosition(tile* Tiles, u32 TileDim, v2i InitialDir, v2i InitialPos)
{
    beam Beams[16384] = {};
    u32 BeamCount = 1;
    u32 BeamIndex = 0;
    Beams[0].Dir = InitialDir;
    Beams[0].Pos = InitialPos;
    Beams[0].Start = InitialPos;
    while (BeamIndex < BeamCount)
    {
        beam* Beam = Beams + BeamIndex;
        tile_type Hit = AdvanceBeam(Beam, Tiles, TileDim);
        // PrintGridAndCalculateEnergized(Tiles, TileDim);
        if (!Beam->Terminated)
        {
            switch (Hit)
            {
                case ForwardMirror:
                {
                    // Flip X <-> Y and negate
                    v2i FlippedDir = {Beam->Dir.Y, Beam->Dir.X};
                    Beam->Dir = -FlippedDir;
                    Beam->Pos += Beam->Dir;
                }
                break;
                case BackwardMirror:
                {
                    // Flip X <-> Y
                    v2i FlippedDir = {Beam->Dir.Y, Beam->Dir.X};
                    Beam->Dir = FlippedDir;
                    Beam->Pos += Beam->Dir;
                }
                break;
                case HorizontalSplitter:
                {
                    if (Beam->Dir.Y != 0)
                    {
                        Beam->Terminated = true;
                        beam* Left = Beams + BeamCount++;
                        Left->Dir = {-1, 0};
                        Left->Pos = Beam->Pos + Left->Dir;
                        Left->Start = Beam->Pos;
                        if (Left->Pos.X < 0)
                        {
                            Left->Terminated = true;
                        }

                        beam* Right = Beams + BeamCount++;
                        Right->Dir = {1, 0};
                        Right->Pos = Beam->Pos + Right->Dir;
                        Right->Start = Beam->Pos;
                        if (Right->Pos.X == TileDim)
                        {
                            Right->Terminated = true;
                        }
                    }
                    else
                    {
                        if (Beam->Pos.X + Beam->Dir.X >= 0 && Beam->Pos.X + Beam->Dir.X < TileDim)
                        {
                            Beam->Pos += Beam->Dir;
                        }
                        else
                        {
                            Beam->Terminated = true;
                        }
                    }
                }
                break;
                case VerticalSplitter:
                {
                    if (Beam->Dir.X != 0)
                    {
                        Beam->Terminated = true;
                        beam* Up = Beams + BeamCount++;
                        Up->Dir = {0, -1};
                        Up->Pos = Beam->Pos + Up->Dir;
                        Up->Start = Beam->Pos;

                        beam* Down = Beams + BeamCount++;
                        Down->Dir = {0, 1};
                        Down->Pos = Beam->Pos + Down->Dir;
                        Down->Start = Beam->Pos;
                    }
                    else
                    {
                        if (Beam->Pos.Y + Beam->Dir.Y >= 0 && Beam->Pos.Y + Beam->Dir.Y < TileDim)
                        {
                            Beam->Pos += Beam->Dir;
                        }
                        else
                        {
                            Beam->Terminated = true;
                        }
                    }
                }
                break;

                case OutOfBounds:
                {
                    Beam->Terminated = true;
                }
                break;

                default:
                {
                    Assert(!"We hit something unexpected");
                }
            }
        }

        if (Beam->Terminated)
        {
            ++BeamIndex;
        }
    }

    u32 Result = PrintGridAndCalculateEnergized(Tiles, TileDim);
    return Result;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/daysixteen.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daysixteen.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        tile Tiles[16384] = {};
        u32 TileCount = 0;

        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                token Symbol = PeekToken(Tokenizer);
                while (Symbol.Type != Token_EndOfStream)
                {
                    tile* Tile = Tiles + TileCount++;
                    Symbol = GetToken(Tokenizer);
                    switch (Symbol.Type)
                    {
                        case Token_Period:
                        {
                            Tile->Type = Empty;
                        }
                        break;

                        case Token_ForwardSlash:
                        {
                            Tile->Type = ForwardMirror;
                        }
                        break;

                        case Token_BackwardSlash:
                        {
                            Tile->Type = BackwardMirror;
                        }
                        break;

                        case Token_Minus:
                        {
                            Tile->Type = HorizontalSplitter;
                        }
                        break;

                        case Token_Pipe:
                        {
                            Tile->Type = VerticalSplitter;
                        }
                        break;

                        default:
                        {
                            Assert(!"Invalid token found");
                        }
                    }
                    Symbol = PeekToken(Tokenizer);
                }
            }
            else
            {
                break;
            }
        }

        // We only work with square inputs, could figure out the dimensions while parsing tokens but didn't seem worth it
        u32 TileDim = (u32)sqrt(TileCount);
        printf("There were %u tiles with a dim of %ux%u\n", TileCount, TileDim, TileDim);

        umm TileMapSize = TileDim * TileDim * sizeof(tile);
        tile* Temp = (tile*)malloc(TileMapSize);

        // Part 1
#if 0
        memcpy(Temp, Tiles, TileMapSize);
        v2i InitialPos = {0, 0};
        v2i InitialDir = {1, 0};
        ProcessGridFromStartingPosition(Temp, TileDim, InitialDir, InitialPos);

#else

        // Part 2
        u32 BestConfigurationTotal = 0;

        for (u32 X = 0; X < TileDim; ++X)
        {
            // South
            memcpy(Temp, Tiles, TileMapSize);
            v2i InitialPos = {(s32)X, 0};
            v2i InitialDir = {0, 1};
            u32 Energized = ProcessGridFromStartingPosition(Temp, TileDim, InitialDir, InitialPos);
            if (Energized > BestConfigurationTotal)
            {
                BestConfigurationTotal = Energized;
            }

            // North
            memcpy(Temp, Tiles, TileMapSize);
            InitialPos = {(s32)X, (s32)(TileDim - 1)};
            InitialDir = {0, -1};
            Energized = ProcessGridFromStartingPosition(Temp, TileDim, InitialDir, InitialPos);
            if (Energized > BestConfigurationTotal)
            {
                BestConfigurationTotal = Energized;
            }
        }

        for (u32 Y = 0; Y < TileDim; ++Y)
        {
            // East
            memcpy(Temp, Tiles, TileMapSize);
            v2i InitialPos = {0, (s32)Y};
            v2i InitialDir = {1, 0};
            u32 Energized = ProcessGridFromStartingPosition(Temp, TileDim, InitialDir, InitialPos);
            if (Energized > BestConfigurationTotal)
            {
                BestConfigurationTotal = Energized;
            }

            // West
            memcpy(Temp, Tiles, TileMapSize);
            InitialPos = {(s32)(TileDim - 1), (s32)Y};
            InitialDir = {-1, 0};
            Energized = ProcessGridFromStartingPosition(Temp, TileDim, InitialDir, InitialPos);
            if (Energized > BestConfigurationTotal)
            {
                BestConfigurationTotal = Energized;
            }
        }

        printf("Best configuration was %u\n", BestConfigurationTotal);
#endif
    }

    return 0;
}

// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>

#include "common/common.h"
#include "common/tokenizer.h"

struct game
{
    u32 Red;
    u32 Green;
    u32 Blue;
};

b32 IsValidGame(game* Test)
{
    game ValidGame = {12, 13, 14};

    b32 Result = Test->Red <= ValidGame.Red && Test->Green <= ValidGame.Green && Test->Blue <= ValidGame.Blue;
    return Result;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile("inputs/daytwo.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daytwo.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        u32 SumValidGameIds = 0;
        u32 SumPowerSets = 0;
        while (Parsing(Tokenizer))
        {
            game CurrentGame = {};

            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                RequireIdentifier(Tokenizer, "Game");
                token GameId = RequireToken(Tokenizer, Token_Number);
                u32 gameId = GameId.Number;
                RequireToken(Tokenizer, Token_Colon);

                while (Parsing(Tokenizer))
                {
                    token Token = PeekToken(Tokenizer);
                    if (Token.Type == Token_EndOfLine)
                    {
                        break;
                    }
                    else if (Token.Type == Token_Number)
                    {
                        Token = GetToken(Tokenizer);
                        token Colour = RequireToken(Tokenizer, Token_Identifier);
                        if (StringsAreEqual(Colour.Text, WrapZ("red")))
                        {
                            CurrentGame.Red = CurrentGame.Red < Token.Number ? Token.Number : CurrentGame.Red;
                        }
                        else if (StringsAreEqual(Colour.Text, WrapZ("green")))
                        {
                            CurrentGame.Green = CurrentGame.Green < Token.Number ? Token.Number : CurrentGame.Green;
                        }
                        else if (StringsAreEqual(Colour.Text, WrapZ("blue")))
                        {
                            CurrentGame.Blue = CurrentGame.Blue < Token.Number ? Token.Number : CurrentGame.Blue;
                        }

                        token Delimiter = PeekToken(Tokenizer);
                        if (Delimiter.Type == Token_EndOfLine || Delimiter.Type == Token_Identifier || Delimiter.Type == Token_EndOfStream)
                        {
                            break;
                        }
                        else
                        {
                            GetToken(Tokenizer);
                        }
                    }
                }

                if (IsValidGame(&CurrentGame))
                {
                    SumValidGameIds += gameId;
                }

                SumPowerSets += CurrentGame.Red * CurrentGame.Green * CurrentGame.Blue;
            }
            else
            {
                break;
            }
        }

        printf("Valid game id sum: %u\n", SumValidGameIds);
        printf("Sum of the power of the sets: %u\n", SumPowerSets);
    }

    return 0;
}

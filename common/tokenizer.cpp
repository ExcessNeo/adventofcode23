#include "tokenizer.h"

#include <cstdarg>
#include <cstdio>

string GetTokenTypeName(token_type Type)
{
    switch (Type)
    {
        case Token_Colon: { return (BundleZ("colon")); }
        case Token_Semicolon: { return (BundleZ("semicolon")); }
        case Token_Comma: { return (BundleZ("comma")); }
        case Token_Period: { return (BundleZ("period")); }
        case Token_Asterisk: { return (BundleZ("asterisk")); }
        case Token_Pound: { return (BundleZ("pound")); }
        case Token_Plus: { return (BundleZ("plus")); }
        case Token_Minus: { return (BundleZ("minus")); }
        case Token_Dollar: { return (BundleZ("dollar")); }
        case Token_ForwardSlash: { return (BundleZ("forward slash")); }
        case Token_BackwardSlash: { return (BundleZ("backward slash")); }
        case Token_Percent: { return (BundleZ("percent")); }
        case Token_Equals: { return (BundleZ("equals")); }
        case Token_At: { return (BundleZ("at")); }
        case Token_And: { return (BundleZ("and")); }
        case Token_Pipe: { return (BundleZ("pipe")); }
        case Token_OpenParen: { return (BundleZ("open paren")); }
        case Token_CloseParen: { return (BundleZ("close paren")); }
        case Token_QuestionMark: { return (BundleZ("question mark")); }

        case Token_Identifier: { return (BundleZ("identifier")); }
        case Token_Number: { return (BundleZ("number")); }

        case Token_Spacing: { return (BundleZ("whitespace")); }
        case Token_EndOfLine: { return (BundleZ("end of line")); }

        case Token_EndOfStream: { return (BundleZ("end of stream")); }
    }

    return (BundleZ("unknown"));
}


b32 Parsing(tokenizer* Tokenizer)
{
    b32 Result = !Tokenizer->Error;
    return Result;
}

void ErrorArgList(tokenizer* Tokenizer, token OnToken, char* Format, va_list ArgList)
{
    printf("%.*s(%u,%u): \"%.*s\"", (s32)OnToken.Filename.Count, (char*)OnToken.Filename.Data, OnToken.LineNumber, OnToken.ColumnNumber, (s32)OnToken.Text.Count, (char*)OnToken.Text.Data);
    vprintf(Format, ArgList);
    printf("\n");

    Tokenizer->Error = true;
}

void Error(tokenizer* Tokenizer, token OnToken, char* Format, ...)
{
    va_list ArgList;
    va_start(ArgList, Format);

    ErrorArgList(Tokenizer, OnToken, Format, ArgList);

    va_end(ArgList);

    Tokenizer->Error = true;
}

void Error(tokenizer* Tokenizer, char* Format, ...)
{
    va_list ArgList;
    va_start(ArgList, Format);

    token OnToken = PeekTokenRaw(Tokenizer);
    ErrorArgList(Tokenizer, OnToken, Format, ArgList);

    va_end(ArgList);

    Tokenizer->Error = true;
}

void Refill(tokenizer* Tokenizer)
{
    if (Tokenizer->Input.Count == 0)
    {
        Tokenizer->At[0] = 0;
        Tokenizer->At[1] = 0;
    }
    else if (Tokenizer->Input.Count == 1)
    {
        Tokenizer->At[0] = Tokenizer->Input.Data[0];
        Tokenizer->At[1] = 0;
    }
    else
    {
        Tokenizer->At[0] = Tokenizer->Input.Data[0];
        Tokenizer->At[1] = Tokenizer->Input.Data[1];
    }
}

b32 TokenEquals(token Token, char* Match)
{
    b32 Result = StringsAreEqual(Token.Text, Match);
    return Result;
}

b32 IsValid(token Token)
{
    b32 Result = Token.Type != Token_Unknown;
    return Result;
}

void AdvanceChars(tokenizer* Tokenizer, int count)
{
    Tokenizer->ColumnNumber += count;
    Advance(&Tokenizer->Input, count);
    Refill(Tokenizer);
}

token GetTokenRaw(tokenizer* Tokenizer)
{
    token Token = {};
    Token.Filename = Tokenizer->Filename;
    Token.ColumnNumber = Tokenizer->ColumnNumber;
    Token.LineNumber = Tokenizer->LineNumber;
    Token.Text = Tokenizer->Input;

    char C = Tokenizer->At[0];
    AdvanceChars(Tokenizer, 1);
    switch (C)
    {
        // @formatter:off
        case '\0': {Token.Type = Token_EndOfStream;} break;
        case ':': {Token.Type = Token_Colon;} break;
        case ';': {Token.Type = Token_Semicolon;} break;
        case ',': {Token.Type = Token_Comma;} break;
        case '.': {Token.Type = Token_Period;} break;
        case '*': {Token.Type = Token_Asterisk;} break;
        case '#': {Token.Type = Token_Pound;} break;
        case '+': {Token.Type = Token_Plus;} break;
        case '$': {Token.Type = Token_Dollar;} break;
        case '/': {Token.Type = Token_ForwardSlash;} break;
        case '\\': {Token.Type = Token_BackwardSlash;} break;
        case '%': {Token.Type = Token_Percent;} break;
        case '=': {Token.Type = Token_Equals;} break;
        case '@': {Token.Type = Token_At;} break;
        case '&': {Token.Type = Token_And;} break;
        case '|': {Token.Type = Token_Pipe;} break;
        case '(': {Token.Type = Token_OpenParen;} break;
        case ')': {Token.Type = Token_CloseParen;} break;
        case '?': {Token.Type = Token_QuestionMark;} break;
        // @formatter:on

        default:
        {
            if (IsSpacing(C))
            {
                Token.Type = Token_Spacing;

                while (IsSpacing(Tokenizer->At[0]))
                {
                    AdvanceChars(Tokenizer, 1);
                }
            }
            else if (IsEndOfLine(C))
            {
                Token.Type = Token_EndOfLine;

                if (C == '\r' && Tokenizer->At[0] == '\n' || C == '\n' && Tokenizer->At[0] == '\r')
                {
                    AdvanceChars(Tokenizer, 1);
                }

                Tokenizer->ColumnNumber = 1;
                ++Tokenizer->LineNumber;
            }
            else if (IsAlpha(C))
            {
                Token.Type = Token_Identifier;

                while (IsAlpha(Tokenizer->At[0]) || Tokenizer->At[0] == '-')
                {
                    AdvanceChars(Tokenizer, 1);
                }
            }
            else if (IsNumber(C) || C == '-' && IsNumber(Tokenizer->At[0]))
            {
                s64 number = 0;
                b32 IsNegative = false;
                if (C == '-')
                {
                    IsNegative = true;
                }
                else
                {
                    number = C - '0';
                }

                while (IsNumber(Tokenizer->At[0]))
                {
                    s64 digit = Tokenizer->At[0] - '0';
                    number = 10 * number + digit;
                    AdvanceChars(Tokenizer, 1);
                }

                Token.Type = Token_Number;
                Token.Number = IsNegative ? -number : number;
            }
            else if (C == '-')
            {
                Token.Type = Token_Minus;
            }
            else
            {
                Token.Type = Token_Unknown;
            }
        }
    }

    Token.Text.Count = Tokenizer->Input.Data - Token.Text.Data;

    return Token;
}

token PeekTokenRaw(tokenizer* Tokenizer)
{
    tokenizer Temp = *Tokenizer;
    token Result = GetTokenRaw(Tokenizer);
    *Tokenizer = Temp;
    return Result;
}

token GetToken(tokenizer* Tokenizer, b32 IgnoreEndLine)
{
    token Token;
    for (;;)
    {
        Token = GetTokenRaw(Tokenizer);
        if (Token.Type == Token_Spacing || (Token.Type == Token_EndOfLine && IgnoreEndLine))
        {
            // NOTE: Ignore these when we're getting "real" tokens
        }
        else
        {
            break;
        }
    }

    return Token;
}
token PeekToken(tokenizer* Tokenizer, b32 IgnoreEndLine)
{
    tokenizer Temp = *Tokenizer;
    token Result = GetToken(&Temp, IgnoreEndLine);
    return Result;
}

token RequireToken(tokenizer* Tokenizer, token_type Type)
{
    token Token = GetToken(Tokenizer);
    if (Token.Type != Type)
    {
        string TokenTypeName = GetTokenTypeName(Type);
        Error(Tokenizer, Token, (char*)"Unexpected token type (expected %.*s)", TokenTypeName.Count, TokenTypeName.Data);
    }
    return Token;
}

token RequireIdentifier(tokenizer* Tokenizer, char* Match)
{
    token Id = RequireToken(Tokenizer, Token_Identifier);
    if (!TokenEquals(Id, Match))
    {
        Error(Tokenizer, Id, (char*)"Expected\"%s\"", Match);
    }
    return Id;
}

tokenizer Tokenize(string Input, string FileName)
{
    tokenizer Result = {};

    Result.Filename = FileName;
    Result.ColumnNumber = 1;
    Result.LineNumber = 1;
    Result.Input = Input;
    Refill(&Result);

    return Result;
}
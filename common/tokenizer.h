#pragma once
#include "common.h"

enum token_type
{
    Token_Unknown,

    Token_Colon,
    Token_Semicolon,
    Token_Comma,
    Token_Period,
    Token_Asterisk,
    Token_Pound,
    Token_Plus,
    Token_Minus,
    Token_Dollar,
    Token_ForwardSlash,
    Token_BackwardSlash,
    Token_Percent,
    Token_Equals,
    Token_At,
    Token_And,
    Token_Pipe,
    Token_OpenParen,
    Token_CloseParen,
    Token_QuestionMark,
    Token_OpenBrace,
    Token_CloseBrace,
    Token_LessThan,
    Token_GreaterThan,

    Token_Identifier,
    Token_Number,

    Token_Spacing,
    Token_EndOfLine,

    Token_EndOfStream,
};

struct token
{
    string Filename;
    u32 ColumnNumber;
    u32 LineNumber;

    token_type Type;
    string Text;
    s64 Number;
};

struct tokenizer
{
    string Filename;
    u32 ColumnNumber;
    u32 LineNumber;

    string Input;
    char At[2];

    b32 Error;
};

b32 Parsing(tokenizer* Tokenizer);
void Error(tokenizer* Tokenizer, token OnToken, char* Format, ...);
void Error(tokenizer* Tokenizer, char* Format, ...);

b32 TokenEquals(token Token, char* Match);
b32 IsValid(token Token);
token GetTokenRaw(tokenizer* Tokenizer);
token PeekTokenRaw(tokenizer* Tokenizer);
token GetToken(tokenizer* Tokenizer, b32 IgnoreEndLine = true);
token PeekToken(tokenizer* Tokenizer, b32 IgnoreEndLine = true);
token RequireToken(tokenizer* Tokenizer, token_type Type);
token RequireIdentifier(tokenizer* Tokenizer, char* Match);
tokenizer Tokenize(string Input, string FileName);
string GetTokenTypeName(token_type Type);
void AdvanceChars(tokenizer* Tokenizer, int count);
b32 OptionalToken(tokenizer* Tokenizer, token_type DesiredType);
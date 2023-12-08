// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "common/common.h"
#include "common/tokenizer.h"

struct node;

struct connection
{
    string Label;
    node* Dest;
};

struct node
{
    string Label;
    connection Left;
    connection Right;
};

void ConnectNodes(node* Nodes, u32 NodeCount, node* Node)
{
    for (u32 NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
    {
        node* Test = Nodes + NodeIndex;
        if (Test != Node)
        {
            if (StringsAreEqual(Test->Label, Node->Left.Label))
            {
                Node->Left.Dest = Test;
            }
            if (StringsAreEqual(Test->Label, Node->Right.Label))
            {
                Node->Right.Dest = Test;
            }
        }
    }
}

u32 CountHops(node* Start, node* Dest, string Directions)
{
    u32 Count = 0;
    node* Current = Start;
    string Temp = Directions;
    while (Current != Dest)
    {
        char Dir = Temp.Data[0];
        Current = Dir == 'L' ? Current->Left.Dest : Current->Right.Dest;
        ++Count;
        Advance(&Temp, 1);

        if (Temp.Count == 0)
        {
            Temp = Directions;
        }
    }

    return Count;
}

s64 Gcd(s64 a, s64 b)
{
    s64 numerator;
    s64 denominator;
    s64 remainder;

    if (a > b)
    {
        numerator = a;
        denominator = b;
    }
    else
    {
        numerator = b;
        denominator = a;
    }
    remainder = numerator % denominator;

    while (remainder != 0)
    {
        numerator = denominator;
        denominator = remainder;
        remainder = numerator % denominator;
    }

    return denominator;
}

s64 Lcm(s64 A, s64 B)
{
    s64 Result = A * B / Gcd(A, B);
    return Result;
}

s64 LcmN(s64 Values[], u32 Count)
{
    s64 temp[Count - 1];

    if (Count == 2)
    {
        return Lcm(Values[0], Values[1]);
    }
    else
    {
        for (u32 i = 0; i < Count - 1; i++)
        {
            temp[i] = Values[i];
        }

        temp[Count - 2] = Lcm(Values[Count - 2], Values[Count - 1]);
        return LcmN(temp, Count - 1);
    }
}

u64 CountWideHops(node** nodes, u32 VisitingCount, string Directions)
{
    s64 Count[6] = {};

    for (u32 Index = 0; Index < VisitingCount; ++Index)
    {
        node** Current = &nodes[Index];

        string Temp = Directions;
        while ((*Current)->Label.Data[2] != 'Z')
        {
            char Dir = Temp.Data[0];
            *Current = Dir == 'L' ? (*Current)->Left.Dest : (*Current)->Right.Dest;
            ++Count[Index];
            Advance(&Temp, 1);

            if (Temp.Count == 0)
            {
                Temp = Directions;
            }
        }
    }

    s64 Result = LcmN(Count, VisitingCount);
    return Result;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile("inputs/dayeight.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/dayeight.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        node Nodes[1024] = {};
        u32 NodeCount = 0;

        token DirectionsToken = RequireToken(Tokenizer, Token_Identifier);
        string Directions;
        Directions = DirectionsToken.Text;

        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                token Label = RequireToken(Tokenizer, Token_Identifier);
                RequireToken(Tokenizer, Token_Equals);
                RequireToken(Tokenizer, Token_OpenParen);
                token LeftLabel = RequireToken(Tokenizer, Token_Identifier);
                RequireToken(Tokenizer, Token_Comma);
                token RightLabel = RequireToken(Tokenizer, Token_Identifier);
                RequireToken(Tokenizer, Token_CloseParen);

                node* Node = Nodes + NodeCount++;
                Node->Label = Label.Text;
                Node->Left.Label = LeftLabel.Text;
                Node->Right.Label = RightLabel.Text;
            }
            else
            {
                break;
            }
        }

        node* Start = nullptr;
        node* End = nullptr;
        node* Visiting[6] = {};
        u32 VisitingCount = 0;
        for (u32 NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
        {
            node* Node = Nodes + NodeIndex;
            ConnectNodes(Nodes, NodeCount, Node);
            if (StringsAreEqual(Node->Label, "AAA"))
            {
                Start = Node;
            }
            if (StringsAreEqual(Node->Label, "ZZZ"))
            {
                End = Node;
            }

            if (Node->Label.Data[2] == 'A')
            {
                Visiting[VisitingCount++] = Node;
            }
        }

        u32 Hops = CountHops(Start, End, Directions);
        printf("There were %u hops in total\n", Hops);

        u64 WideHops = CountWideHops(Visiting, VisitingCount, Directions);
        printf("There were %llu hops in total for part 2\n", WideHops);
    }

    return 0;
}

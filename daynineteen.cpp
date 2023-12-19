// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "common/common.h"
#include "common/tokenizer.h"

enum comparator
{
    C_LessThan,
    C_GreaterThan,
    C_Wildcard,
};

enum outcome
{
    O_Accept,
    O_Reject,
    O_Rule,
};

struct rule
{
    umm ComponentOffset;
    comparator Comparator;
    u32 Value;
    outcome Outcome;
    string DestLabel;
};

struct workflow
{
    string Label;
    rule Rules[4];
    u32 RuleCount;
    workflow* Next;
};

struct part
{
    u32 X;
    u32 M;
    u32 A;
    u32 S;
};

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/daynineteen.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/daynineteen.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        workflow WorkflowHashTable[4096] = {};
        part Parts[512] = {};
        u32 PartCount = 0;

        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                token Token = PeekToken(Tokenizer);
                if (Token.Type == Token_Identifier)
                {
                    // Process rule
                    token Label = RequireToken(Tokenizer, Token_Identifier);
                    u32 HashSlot = StringHashOf(Label.Text) % ArrayCount(WorkflowHashTable);
                    workflow* Workflow = WorkflowHashTable + HashSlot;
                    if (Workflow->RuleCount != 0)
                    {
                        while (Workflow->Next != nullptr)
                        {
                            Workflow = Workflow->Next;
                        }
                        Workflow->Next = (workflow *)malloc(sizeof(workflow));
                        Workflow = Workflow->Next;
                        memset(Workflow, 0, sizeof(workflow));
                    }
                    Workflow->Label = Label.Text;
                    RequireToken(Tokenizer, Token_OpenBrace);
                    Token = PeekToken(Tokenizer);
                    while (Token.Type != Token_CloseBrace)
                    {
                        rule* Rule = Workflow->Rules + Workflow->RuleCount++;

                        token Component = RequireToken(Tokenizer, Token_Identifier);
                        Token = PeekToken(Tokenizer);
                        if (Token.Type == Token_GreaterThan || Token.Type == Token_LessThan)
                        {
                            // Build rule
                            if (Component.Text.Data[0] == 'x')
                            {
                                Rule->ComponentOffset = OffsetOf(part, X);
                            }
                            else if (Component.Text.Data[0] == 'm')
                            {
                                Rule->ComponentOffset = OffsetOf(part, M);
                            }
                            else if (Component.Text.Data[0] == 'a')
                            {
                                Rule->ComponentOffset = OffsetOf(part, A);
                            }
                            else if (Component.Text.Data[0] == 's')
                            {
                                Rule->ComponentOffset = OffsetOf(part, S);
                            }

                            token Comparator = GetToken(Tokenizer);
                            if (Comparator.Type == Token_LessThan)
                            {
                                Rule->Comparator = C_LessThan;
                            }
                            else if (Comparator.Type == Token_GreaterThan)
                            {
                                Rule->Comparator = C_GreaterThan;
                            }
                            token Value = RequireToken(Tokenizer, Token_Number);
                            Rule->Value = Value.Number;
                            RequireToken(Tokenizer, Token_Colon);
                            token Destination = RequireToken(Tokenizer, Token_Identifier);
                            if (StringsAreEqual(Destination.Text, "A"))
                            {
                                Rule->Outcome = O_Accept;
                            }
                            else if (StringsAreEqual(Destination.Text, "R"))
                            {
                                Rule->Outcome = O_Reject;
                            }
                            else
                            {
                                Rule->Outcome = O_Rule;
                                Rule->DestLabel = Destination.Text;
                            }
                            OptionalToken(Tokenizer, Token_Comma);
                            Token = PeekToken(Tokenizer);
                        }
                        else
                        {
                            // Build wildcard rule
                            Rule->Comparator = C_Wildcard;
                            if (StringsAreEqual(Component.Text, "A"))
                            {
                                Rule->Outcome = O_Accept;
                            }
                            else if (StringsAreEqual(Component.Text, "R"))
                            {
                                Rule->Outcome = O_Reject;
                            }
                            else
                            {
                                Rule->Outcome = O_Rule;
                                Rule->DestLabel = Component.Text;
                            }
                        }
                    }
                    GetToken(Tokenizer);
                }
                else if (Token.Type == Token_OpenBrace)
                {
                    // Process part
                    GetToken(Tokenizer);

                    part* Part = Parts + PartCount++;

                    RequireIdentifier(Tokenizer, "x");
                    RequireToken(Tokenizer, Token_Equals);
                    token XValue = RequireToken(Tokenizer, Token_Number);
                    Part->X = XValue.Number;
                    RequireToken(Tokenizer, Token_Comma);

                    RequireIdentifier(Tokenizer, "m");
                    RequireToken(Tokenizer, Token_Equals);
                    token MValue = RequireToken(Tokenizer, Token_Number);
                    Part->M = MValue.Number;
                    RequireToken(Tokenizer, Token_Comma);

                    RequireIdentifier(Tokenizer, "a");
                    RequireToken(Tokenizer, Token_Equals);
                    token AValue = RequireToken(Tokenizer, Token_Number);
                    Part->A = AValue.Number;
                    RequireToken(Tokenizer, Token_Comma);

                    RequireIdentifier(Tokenizer, "s");
                    RequireToken(Tokenizer, Token_Equals);
                    token SValue = RequireToken(Tokenizer, Token_Number);
                    Part->S = SValue.Number;

                    RequireToken(Tokenizer, Token_CloseBrace);
                }
            }
            else
            {
                break;
            }
        }

        string StartLabel = BundleZ("in");
        u32 RatingSum = 0;
        for (u32 PartIndex = 0; PartIndex < PartCount; ++PartIndex)
        {
            string DestLabel = StartLabel;
            part* Part = Parts + PartIndex;
            outcome Result = O_Rule;
            while (Result == O_Rule)
            {
                u32 DestHash = StringHashOf(DestLabel) % ArrayCount(WorkflowHashTable);
                workflow* Current = WorkflowHashTable + DestHash;
                while (!StringsAreEqual(Current->Label, DestLabel))
                {
                    Current = Current->Next;
                }

                for (u32 RuleIndex = 0; RuleIndex < Current->RuleCount; ++RuleIndex)
                {
                    rule* Rule = Current->Rules + RuleIndex;
                    u32 PartValue = *(u32 *)((u8 *)Part + Rule->ComponentOffset);
                    if (Rule->Comparator == C_LessThan && PartValue < Rule->Value)
                    {
                        Result = Rule->Outcome;
                        DestLabel = Rule->DestLabel;
                        break;
                    }
                    else if (Rule->Comparator == C_GreaterThan && PartValue > Rule->Value)
                    {
                        Result = Rule->Outcome;
                        DestLabel = Rule->DestLabel;
                        break;
                    }
                    else if (Rule->Comparator == C_Wildcard)
                    {
                        Result = Rule->Outcome;
                        DestLabel = Rule->DestLabel;
                        break;
                    }
                }
            }
            if (Result == O_Accept)
            {
                RatingSum += Part->X + Part->M + Part->A + Part->S;
            }
        }

        printf("The resulting rating sum was %u\n", RatingSum);
    }

    return 0;
}

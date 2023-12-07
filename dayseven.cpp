// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "common/common.h"
#include "common/tokenizer.h"

#define HAND_SIZE 5

struct hand
{
    u32 HandStrength;
    u32 Bid;
    u32 OrderedHand;
    u32 HandValue;
    string Hand;
};

void CalculateHandStrength(string* Hand, hand* Result, b32 Joker = false)
{
    u32 CardCounts[15] = {};
    u32 HighestCard = 0;
    u32 CardValues = 0;
    for (u32 Index = 0; Index < HAND_SIZE; ++Index)
    {
        u32 CardValue = 0;
        switch ((char)Hand->Data[0])
        {
            // @formatter:off
            case 'A': {CardValue = 14; break;}
            case 'K': {CardValue = 13; break;}
            case 'Q': {CardValue = 12; break;}
            case 'J': {CardValue = Joker ? 1 : 11; break;}
            case 'T': {CardValue = 10; break;}
            default: {CardValue = ((char)Hand->Data[0] - '0'); break;}
            // @formatter:on
        }

        ++CardCounts[CardValue];
        if (HighestCard < CardValue)
        {
            HighestCard = CardValue;
        }
        CardValues |= CardValue;
        CardValues <<= 4;
        Advance(Hand, 1);
    }
    // Shift back 4 because we overshifted with the loop
    CardValues >>= 4;

    b32 PairFound = false;
    b32 SecondPairFound = false;
    b32 TripleFound = false;
    b32 QuadFound = false;
    b32 QuintFound = false;

    // Part 2
    if (Joker)
    {
        u32 NumberOfCopies = 0;
        u32 JokerTarget = 1;
        for (u32 CardIndex = 1; CardIndex < 15; ++CardIndex)
        {
            u32 Count = CardCounts[CardIndex];
            if (CardIndex > JokerTarget && Count >= NumberOfCopies)
            {
                JokerTarget = CardIndex;
                NumberOfCopies = Count;
            }
        }
        CardCounts[JokerTarget] += CardCounts[1];
    }

    u32 OrderedHand = 0;
    u32 Written = 0;
    for (u32 CardIndex = 2; CardIndex < 15; ++CardIndex)
    {
        u32 Count = CardCounts[CardIndex];
        if (PairFound && Count == 2)
        {
            SecondPairFound = true;
        }
        else
        {
            PairFound |= Count == 2;
        }
        TripleFound |= Count == 3;
        QuadFound |= Count == 4;
        QuintFound |= Count == 5;

        for (u32 ShufIndex = 0; ShufIndex < Count; ++ShufIndex)
        {
            u32 Temp = CardIndex << 4 * Written;
            OrderedHand |= Temp;
            ++Written;
        }
    }
    b32 FullHouseFound = TripleFound && PairFound;

    // Originally ORed in the hand value into a big u32 and sorted using a Radix sort but had issues with part 2 with
    // original attempt so changed to use qsort with a comparator, I think now that I changed to a more sensible Joker
    // adjustment strategy it might have worked
    u32 Strength = QuintFound << 5 | QuadFound << 4 | FullHouseFound << 3 | TripleFound << 2 | SecondPairFound << 1 |
                   PairFound;
    Result->HandStrength = Strength;
    Result->OrderedHand = OrderedHand;
    Result->HandValue = CardValues;
}

s32 CompareHands(const void* First, const void* Second)
{
    hand* A = (hand *)First;
    hand* B = (hand *)Second;
    s32 Result = A->HandStrength - B->HandStrength;
    if (Result == 0)
    {
        Result = A->HandValue - B->HandValue;
        if (Result == 0)
        {
            Result = A->OrderedHand - B->OrderedHand;
        }
    }

    return Result;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile("inputs/dayseven.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/dayseven.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        hand Hands[4096] = {};
        hand HandsJoker[4096] = {};
        u32 HandCount = 0;
        while (Parsing(Tokenizer))
        {
            token LineStart = PeekToken(Tokenizer);
            if (LineStart.Type != Token_EndOfStream)
            {
                token HandToken = GetToken(Tokenizer);
                if (HandToken.Text.Count < HAND_SIZE)
                {
                    AdvanceChars(Tokenizer, HAND_SIZE - HandToken.Text.Count);
                    HandToken.Text.Count = 5;
                }
                token BidToken = RequireToken(Tokenizer, Token_Number);

                string Text = HandToken.Text;
                hand* Hand = Hands + HandCount;
                CalculateHandStrength(&HandToken.Text, Hand);
                Hand->Bid = (u32)BidToken.Number;
                Hand->Hand = Text;

                hand* HandJoker = HandsJoker + HandCount++;
                CalculateHandStrength(&Text, HandJoker, true);
                HandJoker->Bid = (u32)BidToken.Number;
                HandJoker->Hand = Hand->Hand;
            }
            else
            {
                break;
            }
        }

        qsort(Hands, HandCount, sizeof(hand), CompareHands);
        qsort(HandsJoker, HandCount, sizeof(hand), CompareHands);

        u32 TotalWinnings = 0;
        u32 TotalWinningsJoker = 0;
        for (u32 Index = 0; Index < HandCount; ++Index)
        {
            u32 Rank = Index + 1;
            hand* Hand = Hands + Index;
            TotalWinnings += Hand->Bid * Rank;
            hand* HandJoker = HandsJoker + Index;
            TotalWinningsJoker += HandJoker->Bid * Rank;
        }
        printf("Total winnings: %u\n", TotalWinnings);
        printf("Total winnings with joker instead of Jack: %u\n", TotalWinningsJoker);
    }

    return 0;
}

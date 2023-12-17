// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>

#include "common/common.h"
#include "common/tokenizer.h"

struct HashEntry
{
    string Label;
    u8 FocalLength;
    HashEntry* Next;
};

u32 HashWord(string Word)
{
    u32 Value = 0;
    while (Word.Count)
    {
        u8 Character = *Advance(&Word, 1);
        Value += Character;
        Value *= 17;
        Value %= 256;
    }
    return Value;
}

s32 main()
{
    buffer FileBuffer = {};
    FileBuffer.Data = GetDataFromFile("inputs/dayfifteen.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {

        HashEntry HashMap[256] = {};
        u32 Part1Sum = 0;
        while (FileBuffer.Count)
        {
            string Word;
            Word.Data = FileBuffer.Data;
            char C = *(char *)Advance(&FileBuffer, 1);
            while (C != ',' && FileBuffer.Count)
            {
                C = *(char *)Advance(&FileBuffer, 1);
            }
            if (C == ',')
            {
                Word.Count = FileBuffer.Data - Word.Data - 1;
            }
            else
            {
                Word.Count = FileBuffer.Data - Word.Data;
            }

            u32 WordValue = HashWord(Word);
            printf("The value of %.*s was %u\n", Word.Count, Word.Data, WordValue);
            Part1Sum += WordValue;

            char LastLetter = Word.Data[Word.Count - 1];
            u8 FocalLength  = 0;

            if (IsNumber(LastLetter))
            {
                FocalLength = LastLetter - '0';
                Word.Count -= 2;
                u32 HashSlot = HashWord(Word);
                HashEntry* Entry = HashMap + HashSlot;
                b32 Result = false;
                while (Entry->Next != nullptr)
                {
                    if (StringsAreEqual(Entry->Label, Word))
                    {
                        Result = true;
                        break;
                    }
                    Entry = Entry->Next;
                }
                Entry->Label = Word;
                Entry->FocalLength = FocalLength;
                if (!Result)
                {
                    Entry->Next = (HashEntry*)malloc(sizeof(HashEntry));
                    *Entry->Next = {};
                }
            }
            else
            {
                Word.Count -= 1;
                u32 HashSlot = HashWord(Word);
                HashEntry* Prev = nullptr;
                HashEntry* Entry = HashMap + HashSlot;
                b32 Result = false;
                while (Entry->Next)
                {
                    if (StringsAreEqual(Entry->Label, Word))
                    {
                        Result = true;
                        break;
                    }
                    else
                    {
                        Prev = Entry;
                        Entry = Entry->Next;
                    }
                }

                if (Result)
                {
                    if (Prev)
                    {
                        Prev->Next = Entry->Next;
                    }
                    else
                    {
                        if (Entry->Next)
                        {
                            *Entry = *Entry->Next;
                        }
                        else
                        {
                            *Entry = {};
                        }
                    }
                }
            }

        }

        printf("The total value is: %u\n", Part1Sum);

        u32 TotalFocusingPower = 0;
        for (u32 BoxIndex = 0; BoxIndex < 256; ++BoxIndex)
        {
            HashEntry* Entry = HashMap + BoxIndex;
            u32 BoxCoefficient = BoxIndex + 1;
            u32 SlotIndex = 1;
            while (Entry->FocalLength != 0)
            {
                u32 FocusingPower = BoxCoefficient * SlotIndex * Entry->FocalLength;
                printf("Focusing power of %.*s was %u\n", Entry->Label.Count, Entry->Label.Data, FocusingPower);
                TotalFocusingPower += FocusingPower;
                ++SlotIndex;
                Entry = Entry->Next;
            }
        }

        printf("The total focusing power was: %u\n", TotalFocusingPower);
    }

    return 0;
}

// ReSharper disable CppStringLiteralToCharPointerConversion
#include <cstdio>
#include <cstdlib>

#include "common/common.h"
#include "common/tokenizer.h"

struct map
{
    u32 Count;
    u32 Dest[64];
    u32 Source[64];
    u32 Len[64];
};

void PushMapValue(map* Map, u32 Dest, u32 Source, u32 Len)
{
    u32 Count = Map->Count;
    Map->Dest[Count] = Dest;
    Map->Source[Count] = Source;
    Map->Len[Count] = Len;
    ++Map->Count;
}

void ParseMapValues(tokenizer* Tokenizer, map* Map)
{
    token Token = PeekToken(Tokenizer);
    while (Token.Type == Token_Number)
    {
        Token = GetToken(Tokenizer);
        u32 Dest = Token.Number;
        Token = GetToken(Tokenizer);
        u32 Source = Token.Number;
        Token = GetToken(Tokenizer);
        u32 Len = Token.Number;

        PushMapValue(Map, Dest, Source, Len);
        Token = PeekToken(Tokenizer);
    }
}

void DebugPrintMap(map* Map)
{
    for (u32 index = 0; index < Map->Count; ++index)
    {
        printf("Dest: %u Source %u Len: %u\n", Map->Dest[index], Map->Source[index], Map->Len[index]);
    }
    printf("\n");
}

u32 FindLocationInMapForValue(u32 Value, map* Map)
{
    u32 Result = Value;
    for (u32 MapIndex = 0; MapIndex < Map->Count; ++MapIndex)
    {
        u32 SourceMin = Map->Source[MapIndex];
        u32 SourceMax = SourceMin + Map->Len[MapIndex];
        if (Value >= SourceMin && Value < SourceMax)
        {
            u32 Offset = Value - SourceMin;
            Result = Map->Dest[MapIndex] + Offset;
        }
    }
    return Result;
}

s32 main()
{
    printf("Hello, World!\n");
    buffer FileBuffer = {};
    FileBuffer.Count = 0;
    FileBuffer.Data = GetDataFromFile((char *)"inputs/dayfive.txt", &FileBuffer.Count);
    if (FileBuffer.Data)
    {
        tokenizer Tokenizer_ = Tokenize(FileBuffer, WrapZ("inputs/dayfive.txt"));
        tokenizer* Tokenizer = &Tokenizer_;

        map SeedToSoilMap = {};
        map SoilToFertilizerMap = {};
        map FertilizerToWaterMap = {};
        map WaterToLightMap = {};
        map LightToTemperatureMap = {};
        map TemperatureToHumidityMap = {};
        map HumidityToLocationMap = {};

        u32 Seeds[32];
        u32 SeedCount = 0;
        while (Parsing(Tokenizer))
        {
            RequireIdentifier(Tokenizer, "seeds");
            RequireToken(Tokenizer, Token_Colon);

            token Token = PeekToken(Tokenizer);
            while (Token.Type == Token_Number)
            {
                Token = GetToken(Tokenizer);
                Seeds[SeedCount++] = Token.Number;
                Token = PeekToken(Tokenizer);
            }

            RequireIdentifier(Tokenizer, "seed-to-soil");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &SeedToSoilMap);

            RequireIdentifier(Tokenizer, "soil-to-fertilizer");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &SoilToFertilizerMap);

            RequireIdentifier(Tokenizer, "fertilizer-to-water");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &FertilizerToWaterMap);

            RequireIdentifier(Tokenizer, "water-to-light");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &WaterToLightMap);

            RequireIdentifier(Tokenizer, "light-to-temperature");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &LightToTemperatureMap);

            RequireIdentifier(Tokenizer, "temperature-to-humidity");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &TemperatureToHumidityMap);

            RequireIdentifier(Tokenizer, "humidity-to-location");
            RequireToken(Tokenizer, Token_Identifier);
            RequireToken(Tokenizer, Token_Colon);
            ParseMapValues(Tokenizer, &HumidityToLocationMap);

            token EndOfStream = GetToken(Tokenizer);
            if (EndOfStream.Type == Token_EndOfStream)
            {
                break;
            }
        }

        printf("Seeds: \n");
        for (u32 index = 0; index < SeedCount; ++index)
        {
            printf("%u ", Seeds[index]);
        }
        printf("\n");

        printf("seed-to-soil: \n");
        DebugPrintMap(&SeedToSoilMap);
        printf("soil-to-fertilizer: \n");
        DebugPrintMap(&SoilToFertilizerMap);
        printf("fertilizer-to-water: \n");
        DebugPrintMap(&FertilizerToWaterMap);
        printf("water-to-light: \n");
        DebugPrintMap(&WaterToLightMap);
        printf("light-to-temperature: \n");
        DebugPrintMap(&LightToTemperatureMap);
        printf("temperature-to-humidity: \n");
        DebugPrintMap(&TemperatureToHumidityMap);
        printf("humidity-to-location: \n");
        DebugPrintMap(&HumidityToLocationMap);

        // Part 1
        u32 LowestLocation = UINT32_MAX;
        for (u32 SeedIndex = 0; SeedIndex < SeedCount; ++SeedIndex)
        {
            u32 Seed = Seeds[SeedIndex];
            u32 Soil = FindLocationInMapForValue(Seed, &SeedToSoilMap);
            u32 Fertilizer = FindLocationInMapForValue(Soil, &SoilToFertilizerMap);
            u32 Water = FindLocationInMapForValue(Fertilizer, &FertilizerToWaterMap);
            u32 Light = FindLocationInMapForValue(Water, &WaterToLightMap);
            u32 Temperature = FindLocationInMapForValue(Light, &LightToTemperatureMap);
            u32 Humidity = FindLocationInMapForValue(Temperature, &TemperatureToHumidityMap);
            u32 Location = FindLocationInMapForValue(Humidity, &HumidityToLocationMap);

            // printf("Seed: %u Planted in location: %u\n", Seed, Location);
            if (Location < LowestLocation)
            {
                LowestLocation = Location;
            }
        }

        printf("Lowest location was: %u\n", LowestLocation);

        printf("Part 2: \n");

        // Part 2
        LowestLocation = UINT32_MAX;
        for (u32 SeedIndex = 0; SeedIndex < SeedCount / 2; SeedIndex += 2)
        {
            u32 SeedMin = Seeds[SeedIndex];
            u32 SeedLen = Seeds[SeedIndex + 1];
            u32 SeedMax = SeedMin + SeedLen;

            for (u32 Seed = SeedMin; Seed < SeedMax; ++Seed)
            {
                u32 Soil = FindLocationInMapForValue(Seed, &SeedToSoilMap);
                u32 Fertilizer = FindLocationInMapForValue(Soil, &SoilToFertilizerMap);
                u32 Water = FindLocationInMapForValue(Fertilizer, &FertilizerToWaterMap);
                u32 Light = FindLocationInMapForValue(Water, &WaterToLightMap);
                u32 Temperature = FindLocationInMapForValue(Light, &LightToTemperatureMap);
                u32 Humidity = FindLocationInMapForValue(Temperature, &TemperatureToHumidityMap);
                u32 Location = FindLocationInMapForValue(Humidity, &HumidityToLocationMap);

                // printf("Seed: %u Planted in location: %u\n", Seed, Location);
                if (Location < LowestLocation)
                {
                    LowestLocation = Location;
                }
            }
        }

        printf("Lowest location was: %u\n", LowestLocation);

    }

    return 0;
}

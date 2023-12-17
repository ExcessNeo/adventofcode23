#include "common.h"

#include <cstdio>
#include <cstdlib>

u8* GetDataFromFile(char* Filename, umm* Count)
{
    FILE* File = fopen(Filename, "rb");
    *Count = 0;
    u8* data = nullptr;
    if (File)
    {
        fseek(File, 0, SEEK_END);
        *Count = ftell(File);
        fseek(File, 0, SEEK_SET);

        data = (u8*)malloc(*Count + 1);
        fread(data, *Count, 1, File);
        data[*Count] = '\0';
    }

    return data;
}
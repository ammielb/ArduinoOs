#include "EEPROMHandeling.h"
#include <Arduino.h>
void writeFATEntry(int position, fileInfo data)
{
    noOfFiles++;
    EEPROM.put(position, data);
}

fileInfo readFATEntry(int position)
{
    fileInfo result;
    EEPROM.get(position, result);
    return result;
}
void eraseFATEntry(int position)
{
    for (int i = position; i < position + 16; i++)
    {
        EEPROM.write(i, 0);
    }
    // noOfFiles--;
}

// overloaded
void moveFATEntryUp(int originalPosition, fileInfo data)
{
    EEPROM.put(originalPosition - 16, data);
}
// overloaded
void moveFATEntryUp(int originalPosition)
{
    fileInfo data = readFATEntry(originalPosition);
    EEPROM.put(originalPosition - 16, data);
}

void putIntoEEPROM(char *data, int startingPos)
{
    for (int i = 0; i < strlen(data); i++)
    {
        EEPROM.write(startingPos + i, data[i]);
    }
    EEPROM.write(startingPos + strlen(data), '\0'); // optional null-terminator
}

void eraseFromEEPROM(int startingPos, int length)
{
    for (int i = 0; i < length; i++)
    {
        EEPROM.write(startingPos + i, 0);
    }
    EEPROM.write(startingPos + length, '\0'); // optional null-terminator
}
char *retrieveFromEEPROM(fileInfo file)
{
    char *asciiLetter = new char[file.length + 1]; // +1 for null terminator

    for (size_t i = 0; i < file.length; i++)
    {
        asciiLetter[i] = EEPROM.read(file.position + i);
    }
    asciiLetter[file.length] = '\0'; // Null-terminate the string if needed

    return asciiLetter;
    delete[] asciiLetter;
}
int findName(char *name)
{
    // Serial.println(noOfFiles);
    for (int i = 0; i < noOfFiles; i++)
    {
        //  index + fileInfosize + translation for numberOfFiles variable
        int positionIndex = i * 16 + 1;
        fileInfo file = readFATEntry(positionIndex);

        if (strcmp(file.name, name) == 0)
        {
            return i * 16 + 1;
        }
    }

    return -1;
}
int findAvailableSpace(byte size)
{
    const int FAT_start = 1;
    const int file_info_size = sizeof(fileInfo);

    byte totalFiles = noOfFiles;
    if (totalFiles == 0)
    {
        return 161;
    }

    if (totalFiles == 1)
    {
        fileInfo onlyFile = readFATEntry(FAT_start);
        int spaceLeft = EEPROM.length() - (onlyFile.position + onlyFile.length);
        if (spaceLeft >= size)
        {
            return (onlyFile.position + onlyFile.length);
        }
        return -1;
    }

    //  check if there is available space left afterwards.
    int lastAddr = FAT_start + (totalFiles - 1) * file_info_size;
    fileInfo lastEntry = readFATEntry(lastAddr);

    int spaceLeft = EEPROM.length() - (lastEntry.position + lastEntry.length);
    if (spaceLeft >= size)
    {
        return (lastEntry.position + lastEntry.length);
    }
    return -1;
}
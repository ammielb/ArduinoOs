#include <Arduino.h>
#include "EEPROM.h"
#include "commands.h"
#include "EEPROMHandeling.h"
#include "procesHandeling.h"
commandType commands[] = {
    {"store", store},
    {"erase", erase},
    {"retrieve", retrieve},
    {"files", files},
    {"freespace", freespace},
    {"showEEPROM", showEEPROM},
    {"run", run},
    {"suspend", suspend},
    {"resume", resume},
    {"kill", kill},
    {"list", list},
    {"clearEEPROM", clearEEPROM},
    {"showByte", showProcesByteCode},
};
const int nCommands = sizeof(commands) / sizeof(commandType);
void commandHandler(const char **userInput)
{
    const char *command = userInput[0];
    const char *args = userInput[1];
    for (int i = 0; i < nCommands; i++)
    {

        if (strcmp(command, commands[i].name) == 0)
        {
            commands[i].func(args);
            return;
        }
    }
    unknownCommand(args);
}

//
// @param
// char[12} name, byte size, data
void store(const char *arg)
{
    char input[64];
    strncpy(input, arg, sizeof(input));
    input[sizeof(input) - 1] = '\0';

    char *nameToken = strtok(input, " ");
    char *sizeToken = strtok(NULL, " ");

    if (!nameToken || !sizeToken)
    {
        Serial.println(F("Not enough arguments for store."));
        return;
    }
    int size = atoi(sizeToken);

    if (noOfFiles == 10)
    {
        Serial.println(F("No more then 10 files can be saved at a time."));
        return;
    }
    if (findName(nameToken) != -1)
    {
        Serial.println(F("File name already exists. Please enter a different file name."));
        return;
    }
    int availableSpaceIndex = findAvailableSpaceFAT(size);
    if (availableSpaceIndex == -1)
    {
        Serial.println(F("There is no available space left for this file size."));
        return;
    }

    // Prepare file info
    char name[12];
    strncpy(name, nameToken, sizeof(name));
    name[sizeof(name) - 1] = '\0';

    fileInfo file = {0};
    strncpy(file.name, name, sizeof(file.name));
    file.name[sizeof(file.name) - 1] = '\0';
    file.position = availableSpaceIndex;
    file.length = size;

    writeFATEntry(noOfFiles * 16 + 1, file);

    // Now read the raw bytes from Serial
    byte dataBytes[128];
    int dataCount = 0;

    Serial.println(size);
    Serial.println(dataCount);

    char *data = strtok(NULL, "");
    if (data == NULL)
    {

        unsigned long start = millis();
        while (dataCount < size && (millis() - start) < 2000)
        { // 2s timeout
            if (Serial.available())
            {
                dataBytes[dataCount++] = Serial.read();
            }
        }
        if (dataCount != size)
        {
            Serial.println(F("Did not receive all data bytes."));
            return;
        }
    }
    else
    {
        size_t dataLen = strlen(data);
        if (dataLen > (size_t)size)
            dataLen = size;
        memcpy(dataBytes, data, dataLen);
        dataCount = dataLen;
    }

    putIntoEEPROM(dataBytes, size, availableSpaceIndex);
    Serial.println(F("Succesfully inserted file into FAT"));
}
struct TempFile
{
    fileInfo info;
    byte *data;
};

void erase(const char *arg)
{

    if (arg == NULL)
    {
        Serial.println(F("Not enough arguments has been given. Please try again."));
        return;
    }

    int nameIndex = findName(arg);
    if (nameIndex == -1)
    {
        Serial.println(F("No file exists with that name"));
        return;
    }
    fileInfo deletedFile = readFATEntry(nameIndex);

    // Gather all following files' data BEFORE erasing anything else
    const int lastEntryPosition = noOfFiles * 16 + 1;
    int shiftStart = nameIndex + 16;
    int shiftCount = (lastEntryPosition - shiftStart) / 16;

    TempFile *shiftedFiles = new TempFile[shiftCount];
    int fileIdx = 0;

    for (int i = shiftStart; i < lastEntryPosition; i += 16)
    {
        fileInfo file = readFATEntry(i);
        if (file.length == 0)
            continue;

        byte *fileData = retrieveFromEEPROM(file);
        shiftedFiles[fileIdx++] = {file, fileData};
    }

    // erase the FAT and EEPROM content of the deleted file
    eraseFATEntry(nameIndex);
    eraseFromEEPROM(deletedFile.position, deletedFile.length);
    int totalFilesBeforeErasing = noOfFiles;
    // Now re-insert the files at their new positions
    for (int j = 0; j < fileIdx; j++)
    {
        // erase old fileData and insert it into its new position
        eraseFromEEPROM(shiftedFiles[j].info.position, shiftedFiles[j].info.length);
        shiftedFiles[j].info.position -= deletedFile.length;
        putIntoEEPROM(shiftedFiles[j].data, shiftedFiles[j].info.length, shiftedFiles[j].info.position);

        // shift the fat up
        shiftedFiles[j].info.position = deletedFile.position + j * shiftedFiles[j].info.length;
        writeFATEntry(nameIndex + (j * 16), shiftedFiles[j].info);
        eraseFATEntry(nameIndex + 16 + (j * 16)); // clean up old FAT
        delete[] shiftedFiles[j].data;
    }

    delete[] shiftedFiles;

    noOfFiles = totalFilesBeforeErasing - 1;
    Serial.println(F("File erased and remaining files moved."));
}

void retrieve(const char *arg)
{
    if (arg == NULL)
    {
        Serial.println(F("Not enough arguments has been given. Please try again."));
        return;
    }

    int nameIndex = findName(arg);
    if (nameIndex == -1)
    {
        Serial.println(F("No file exists with that name"));
        return;
    }

    fileInfo file = readFATEntry(nameIndex);
    byte *fileData = retrieveFromEEPROM(file);
    for (size_t i = 0; i < file.length; i++)
    {
        byte b = fileData[i];
        if (b >= 32 && b <= 126)
        {
            Serial.print((char)b); // printable ASCII
        }
        else
        {
            Serial.print(b);
        }
    }

    Serial.println(" ");
}
void files(const char *arg)
{
    Serial.println("Number of files: ");
    Serial.println(noOfFiles);
    for (int i = 0; i < noOfFiles; i++)
    {
        //  index + fileInfosize + translation for numberOfFiles variable
        int positionIndex = i * 16 + 1;
        fileInfo file = readFATEntry(positionIndex);
        Serial.print("Name: ");
        Serial.println(file.name);
        Serial.print("size: ");
        Serial.println(file.length);
        Serial.print("Position: ");
        Serial.println(file.position);
        Serial.println(" ");
    }
}
void freespace(const char *arg)
{
    int lastEntry = (noOfFiles - 1) * 16 + 1;
    fileInfo file = readFATEntry(lastEntry);
    Serial.print("Amount of space left is: ");
    Serial.println(1024 - (file.position + file.length));
}
void showCharacterEEPROM(const char *arg)
{
    for (int i = 161; i < EEPROM.length(); i++)
    {
        byte b = EEPROM.read(i);
        if (b >= 32 && b <= 126)
        {
            Serial.print((char)b); // printable ASCII
        }
        else
        {
            Serial.print(".");
        }
    }
    Serial.println();
}
void showEEPROM(const char *arg)
{

    for (int i = 0; i < EEPROM.length(); i++)
    {
        byte b = EEPROM.read(i);
        if (b >= 32 && b <= 126)
        {
            Serial.print((char)b); // printable ASCII
        }
        else
        {
            Serial.print(b);
        }
    }
    Serial.println();
}
void unknownCommand(const char *arg)
{
    Serial.println("Onbekend commando. Beschikbare commando's:");
    for (int i = 0; i < nCommands; i++)
    {
        Serial.println(commands[i].name);
    }
}

void clearEEPROM(const char *arg)
{
    for (int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }
    noOfFiles = 0;
    Serial.println("EEPROM cleared");
}
void showProcesByteCode(const char *arg)
{
    if (arg == NULL)
    {
        Serial.println(F("Not enough arguments has been given. Please try again."));
        return;
    }

    int nameIndex = findName(arg);
    if (nameIndex == -1)
    {
        Serial.println(F("No instruction file exists with that name"));
        return;
    }

    fileInfo file = readFATEntry(nameIndex);
    byte *fileData = retrieveFromEEPROM(file);
    for (size_t i = 0; i < file.length; i++)
    {
        Serial.println(fileData[i]);
    }
}

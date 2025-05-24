#include <Arduino.h>
#include "EEPROM.h"
#include "commands.h"
#include "EEPROMHandeling.h"

commandType commands[] = {
    {"store", store},
    {"erase", erase},
    {"retrieve", retrieve},
    {"files", files},
    {"freespace", freespace},
    {"showEEPROM", showEEPROM},

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
    char *input = arg;
    // split the args into there respective variables
    char *nameToken = strtok(input, " ");
    int size = atoi(strtok(NULL, " "));
    char *data = strtok(NULL, " ");
    if (noOfFiles == 10)
    {
        Serial.println(F("No more then 10 files can be saved at a time."));
        return;
    }
    // check if enough arguments have been passed through
    if (data == NULL)
    {
        Serial.println(F("Not enough arguments has been given. Please try again."));
        return;
    }

    if (findName(nameToken) != -1)
    {
        Serial.println(F("File name already exists. Please enter a different file name."));
        return;
    }

    int availableSpaceIndex = findAvailableSpace(size);
    if (availableSpaceIndex == -1)
    {
        Serial.println(F("There is no available space left for this file size."));
        return;
    }

    //  put data of file into EEPROM
    // Create a fixed-size char array for the name and copy safely
    char name[12];
    strncpy(name, nameToken, sizeof(name));
    name[sizeof(name) - 1] = '\0'; // Ensure null-termination

    // Now create your fileInfo struct with the fixed name array
    fileInfo file = {0}; // zero-init to be safe
    strncpy(file.name, name, sizeof(file.name));
    file.position = availableSpaceIndex;
    file.length = size;

    // storing into the FAT.
    writeFATEntry(noOfFiles * 16 + 1, file);

    // storing the data in the EEPROM
    putIntoEEPROM(data, availableSpaceIndex);
    Serial.println(F(" Succesfully inserted file into FAT"));
}
struct TempFile
{
    fileInfo info;
    char *data;
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

        char *fileData = retrieveFromEEPROM(file);
        shiftedFiles[fileIdx++] = {file, fileData};
    }

    // erase the FAT and EEPROM content of the deleted file
    eraseFATEntry(nameIndex);
    eraseFromEEPROM(deletedFile.position, deletedFile.length);

    // Now re-insert the files at their new positions
    for (int j = 0; j < fileIdx; j++)
    {
        // erase old fileData and insert it into its new position
        eraseFromEEPROM(shiftedFiles[j].info.position, shiftedFiles[j].info.length);
        shiftedFiles[j].info.position -= deletedFile.length;
        putIntoEEPROM(shiftedFiles[j].data, shiftedFiles[j].info.position);

        // shift the fat up
        shiftedFiles[j].info.position = deletedFile.position + j * shiftedFiles[j].info.length;
        writeFATEntry(nameIndex + (j * 16), shiftedFiles[j].info);
        eraseFATEntry(nameIndex + 16 + (j * 16)); // clean up old FAT

        delete[] shiftedFiles[j].data;
    }

    delete[] shiftedFiles;

    noOfFiles--;
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
    Serial.println(retrieveFromEEPROM(file));
}
void files(const char *arg)
{
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
#ifndef EEPROMHANDELING_H
#define EEPROMHANDELING_H
#include <EEPROM.h>
#include <Arduino.h>

// EEPROM address to store file count
static EERef noOfFiles = EEPROM[0];

typedef struct
{
    char name[12];
    int position;
    int length;
} fileInfo;

fileInfo readFATEntry(int position);
void writeFATEntry(int position, fileInfo data);
void moveFATEntryUp(int originalPosition, fileInfo data);
void moveFATEntryUp(int originalPosition);
void eraseFATEntry(int position);

int findName(char *name);
int findAvailableSpace(byte size);

void putIntoEEPROM(char *data, int startingPos);
void eraseFromEEPROM(int startingPos, int length);
char *retrieveFromEEPROM(fileInfo file);

#endif
#include "procesHandeling.h"
#include "EEPROMHandeling.h"
#define MAX_PROCES 10
procesEntry procesTable[MAX_PROCES];
int noOfProces = 0;
int procesID = 0;

void run(const char *arg)
{
    if (noOfProces >= MAX_PROCES)
    {
        Serial.println(F("Not enough space for another proces to be started."));
        return;
    }
    int fileIndex = findName(arg[0]);
    if (fileIndex == -1)
    {
        Serial.println(F("No file exists with that name"));
        return;
    }

    fileInfo file = readFATEntry(fileIndex);
    procesTable[noOfProces].name = arg[0];
    procesTable[noOfProces].procesID = procesID++;
    procesTable[noOfProces].state = (byte) "r";
    procesTable[noOfProces].programCounter = file.position;
    procesTable[noOfProces].stackPointer = 0;
    procesTable[noOfProces].beginningAdres = file.position;
    noOfProces++;
    //  hele stack in proces table
    //  stack grote 32

    Serial.println(F("Process succesfully started."));
}
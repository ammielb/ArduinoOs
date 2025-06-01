#ifndef PROCESHANDELING_H
#define PROCESHANDELING_H

#include <Arduino.h>

struct procesEntry
{
    char name[12];
    int procesID;
    byte state;
    int programCounter;
    int stackPointer;
    int beginningAdres;
};
void run(const char *name);
#endif

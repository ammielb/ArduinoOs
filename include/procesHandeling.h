#ifndef PROCESHANDELING_H
#define PROCESHANDELING_H
#include "stack.h"
#include <Arduino.h>
#define MAX_PROCES 10

struct procesEntry
{
    char name[12];
    int procesID;
    byte state;
    int programCounter;
    int beginningAdres;
    byte *stack;
    int sp;
};
extern procesEntry procesTable[MAX_PROCES];
void run(const char *arg);
void suspend(const char *arg);
void resume(const char *arg);
void kill(const char *arg);
void list(const char *arg);

void changeState(int ID, byte desiredState);
int findID(int ID);

void runProcesses();
void execute(int i);
#endif

#ifndef MEMORYHANDELING_H
#define MEMORYHANDELING_H
#include <Arduino.h>

typedef struct
{
    char name[12];
    int position;
    int length;
} fileInfo;

#endif
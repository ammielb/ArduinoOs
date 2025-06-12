#ifndef STACK_H
#define STACK_H

#include <Arduino.h>

#define STACKSIZE 32

// Basic byte operations
void pushByte(byte b, int procesID);
byte popByte(int procesID);

// Push functions for data types
void pushInt(int data);
void pushFloat(float data);
void pushChar(char data);
void pushString(char *data);

// Pop functions for data types
int popInt();
float popFloat();
char popChar();
char *popString();

void showStack();
#endif
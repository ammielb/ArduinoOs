#include "stack.h"
#include "procesHandeling.h"

void pushByte(byte b, int procesID)
{
    procesEntry proces = procesTable[procesID];
    if (proces.sp >= STACKSIZE)
    {
        Serial.println("Stack overflow!");
        return;
    }
    proces.stack[proces.sp++] = b;
}

byte popByte(int procesID)
{
    procesEntry proces = procesTable[procesID];
    if (proces.sp == 0)
    {
        Serial.println("Stack underflow!");
        return 0;
    }
    return proces.stack[--proces.sp];
}

// push values to stack
void pushInt(int data, int procesID)
{
    byte MSB = highByte(data);
    byte LSB = lowByte(data);
    // value
    pushByte(MSB, procesID);
    pushByte(LSB, procesID);
    //  meta data
    // pushByte(2);
    pushByte('I', procesID);
}

void pushFloat(float data, int procesID)
{
    byte *b = (byte *)&data;

    // value
    pushByte(b[0], procesID);
    pushByte(b[1], procesID);
    pushByte(b[2], procesID);
    pushByte(b[3], procesID);
    //  meta data
    // pushByte(4);
    pushByte('F', procesID);
}

void pushChar(char data, int procesID)
{
    // value
    pushByte(data, procesID);
    //  meta data
    // pushByte(1);
    pushByte('C', procesID);
}

void pushString(char *data, int procesID)
{
    // value
    for (int i = 0; i < strlen(data); i++)
    {
        pushByte(data[i], procesID);
    }
    // terminating zero
    pushByte('\0', procesID);
    //  meta data
    pushByte(strlen(data) + 1, procesID);
    pushByte('S', procesID);
}

// pop values from stack

int popInt(int procesID)
{
    // value
    byte LSB = popByte(procesID);
    byte MSB = popByte(procesID);
    return word(LSB, MSB);
}

float popFloat(int procesID)
{
    // value
    byte b[4];
    b[3] = popByte(procesID);
    b[2] = popByte(procesID);
    b[1] = popByte(procesID);
    b[0] = popByte(procesID);

    float value;
    memcpy(&value, b, sizeof(float));
    return value;
}

char popChar(int procesID)
{
    return popByte(procesID);
}

char *popString(int procesID)
{
    static char result[64];          // static buffer
    byte length = popByte(procesID); // get length first
    for (int i = length - 1; i >= 0; i--)
    {
        result[i] = popByte(procesID);
    }
    return result;
}
int popNumber(int procesID)
{
    char popType = popByte(procesID);
    if (popType == 'F')
    {
        return (int)popFloat(procesID);
    }
    else if (popType == 'I')
    {
        return popInt(procesID);
    }
}
float popVal(int procesID)
{
    char popType = popByte(procesID);
    if (popType == 'F')
    {
        return (int)popFloat(procesID);
    }
    else if (popType == 'I')
    {
        return popInt(procesID);
    }
    else if (popType == 'C')
    {
        return (float)popChar(procesID) - '0';
    }
}
void handleValue(int procesID, )
{
}
void showStack(int procesID)
{
    procesEntry proces = procesTable[procesID];
    for (size_t i = 0; i < proces.sp; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(proces.stack[i], HEX);
    }
}
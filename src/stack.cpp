#include "stack.h"
byte stack[STACKSIZE]; // <-- Add this line
byte sp = 0;

void pushByte(byte b)
{
    if (sp >= STACKSIZE)
    {
        Serial.println("Stack overflow!");
        return;
    }
    stack[sp++] = b;
}

byte popByte()
{
    if (sp == 0)
    {
        Serial.println("Stack underflow!");
        return 0;
    }
    return stack[--sp];
}

// push values to stack
void pushInt(int data)
{
    byte MSB = highByte(data);
    byte LSB = lowByte(data);
    // value
    pushByte(MSB);
    pushByte(LSB);
    //  meta data
    // pushByte(2);
    pushByte('I');
}

void pushFloat(float data)
{
    byte *b = (byte *)&data;

    // value
    pushByte(b[0]);
    pushByte(b[1]);
    pushByte(b[2]);
    pushByte(b[3]);
    //  meta data
    // pushByte(4);
    pushByte('F');
}

void pushChar(char data)
{
    // value
    pushByte(data);
    //  meta data
    // pushByte(1);
    pushByte('C');
}

void pushString(char *data)
{
    // value
    for (int i = 0; i < strlen(data); i++)
    {
        pushByte(data[i]);
    }
    // terminating zero
    pushByte('\0');
    //  meta data
    pushByte(strlen(data) + 1);
    pushByte('S');
}

// pop values from stack

int popInt()
{
    // value
    byte LSB = popByte();
    byte MSB = popByte();
    return word(LSB, MSB);
}

float popFloat()
{
    // value
    byte b[4];
    b[3] = popByte();
    b[2] = popByte();
    b[1] = popByte();
    b[0] = popByte();

    float value;
    memcpy(&value, b, sizeof(float));
    return value;
}

char popChar()
{
    return popByte();
}

char *popString()
{
    static char result[128]; // static buffer
    byte length = popByte(); // get length first
    for (int i = length - 1; i >= 0; i--)
    {
        result[i] = popByte();
    }
    return result;
}
void showStack()
{
    for (size_t i = 0; i < sp; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(stack[i], HEX);
    }
}
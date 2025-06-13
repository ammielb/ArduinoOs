#include "procesHandeling.h"
#include "memoryHandeling.h"
#include "EEPROMHandeling.h"
#include "instruction_set.h"
#include <math.h>
int noOfProces = 0;
int procesID = 0;
procesEntry procesTable[MAX_PROCES];
void run(const char *arg)
{
    if (noOfProces >= MAX_PROCES)
    {
        Serial.println(F("Not enough space for another proces to be started."));
        return;
    }
    int fileIndex = findName(arg);
    if (fileIndex == -1)
    {
        Serial.println(F("No file exists with that name"));
        return;
    }

    fileInfo file = readFATEntry(fileIndex);
    byte newStack[STACKSIZE];
    strncpy(procesTable[noOfProces].name, arg, sizeof(procesTable[noOfProces].name) - 1);
    procesTable[noOfProces].name[sizeof(procesTable[noOfProces].name) - 1] = '\0';
    procesTable[noOfProces].procesID = procesID++;
    procesTable[noOfProces].state = 'r';
    procesTable[noOfProces].programCounter = file.position;
    procesTable[noOfProces].programLength = file.length;
    procesTable[noOfProces].beginningAdres = file.position;
    procesTable[noOfProces].stack = newStack;
    procesTable[noOfProces].sp = 0;
    noOfProces++;

    //  hele stack in proces table
    //  stack grote 32

    Serial.println(F("Process succesfully started."));
}

void suspend(const char *arg)
{
    if (!isDigit(arg[0]))
    {
        Serial.println(F("Given argument is not a valid integer."));
        return;
    }
    Serial.println(arg);
    int procesIndex = findID(atoi(arg));
    if (procesIndex == -1)
    {
        Serial.println(F("Given process ID does not exist"));
        return;
    }

    if (procesTable[procesIndex].state == '0')
    {
        Serial.println(F("Proces already terminated"));
        return;
    }

    changeState(procesIndex, 'p');
}
void resume(const char *arg)
{
    if (!isDigit(arg[0]))
    {
        Serial.println(F("Given argument is not a valid integer."));
        return;
    }
    int procesIndex = findID((int)arg[0]);
    if (procesIndex == -1)
    {
        Serial.println(F("Given process ID does not exist"));
        return;
    }

    if (procesTable[procesIndex].state == '0')
    {
        Serial.println(F("Proces already terminated"));
        return;
    }

    changeState(procesIndex, 'r');
}

void kill(const char *arg)
{
    if (!isDigit(arg[0]))
    {
        Serial.println(F("Given argument is not a valid integer."));
        return;
    }
    Serial.println("killing shit");
    int procesIndex = findID(atoi(arg[0]));
    if (procesIndex == -1)
    {
        Serial.println(F("Given process ID does not exist"));
        return;
    }

    changeState(procesIndex, '0');
    deleteProcesVars(procesIndex);
    removeProcesFromList(procesID);
}
void list(const char *arg)
{
    for (size_t i = 0; i < noOfProces; i++)
    {
        if (procesTable[i].state == '0')
        {
            continue;
        }
        Serial.print(F("ID: "));
        Serial.println(procesTable[i].procesID);
        Serial.print(F("Name: "));
        Serial.println(procesTable[i].name);
        Serial.print(F("State: "));
        Serial.println(procesTable[i].state);
    }
}
// helpers functions
void changeState(int ID, byte desiredState)
{
    if (procesTable[ID].state == desiredState)
    {
        Serial.println(F("Proces already in desired state "));
        return;
    }

    procesTable[ID].state = desiredState;
    Serial.println(F("Changed state of proces "));
}

int findID(int ID)
{
    for (size_t i = 0; i < noOfProces; i++)
    {
        if (procesTable[i].procesID == ID)
        {
            return i;
        }
    }
    return -1;
}
void removeProcesFromList(int procesIndex)
{

    for (size_t i = procesIndex; i < noOfProces - 1; i++)
    {
        // move everything back  by 1from the point of hte deleted proces
        procesTable[i] = procesTable[i + 1];
    }
    procesTable[noOfProces - 1] = procesEntry(); // Reset the last entry
    noOfProces--;
    procesID--;
}
//
// running processes and executing instructions
//
void runProcesses()
{
    // Serial.println((char)procesTable[0].state);
    // Serial.println(noOfProces);
    for (size_t i = 0; i <= noOfProces; i++)
    {

        execute(i);
    }
}
void execute(int i)
{

    if (procesTable[i].state == 'r')
    {

        char *name = procesTable[i].name;

        int procesCounter = procesTable[i].programCounter;
        int beginAdres = procesTable[i].beginningAdres;
        int length = procesTable[i].programLength;
        int procesID = procesTable[i].procesID;

        byte instruction = EEPROM.read(procesCounter);

        int x;
        int y;
        int z;
        int a;
        int b;

        char charX;
        char charY;

        float floatX;
        float floatY;
        float floatZ;
        float floatA;
        float floatB;

        char popType;
        switch (instruction)
        {
        case CHAR:
            Serial.println(F("Executing CHAR"));
            break;

        case INT:
            Serial.println(F("Executing INT"));
            break;

        case STRING:
            Serial.println(F("Executing STRING"));
            break;

        case FLOAT:
            Serial.println(F("Executing FLOAT"));
            break;

        case SET:
            Serial.println(F("Executing SET"));
            popByte(procesID); // get type;
            setVar(popChar(procesID), procesID);
            break;

        case GET:
            Serial.println(F("Executing GET"));
            popByte(procesID); // get type;
            getVar(popChar(procesID), procesID);
            break;

        case INCREMENT:
            Serial.println(F("Executing INCREMENT"));
            popByte(procesID); // get type;
            pushInt(popInt(procesID) + 1, procesID);
            break;

        case DECREMENT:
            Serial.println(F("Executing DECREMENT"));
            popByte(procesID); // get type;
            pushInt(popInt(procesID) - 1, procesID);
            break;

        case PLUS:
            Serial.println(F("Executing PLUS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);

            pushInt(x + y, procesID);
            break;

        case MINUS:
            Serial.println(F("Executing MINUS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);

            pushInt(x - y, procesID);
            break;

        case TIMES:
            Serial.println(F("Executing TIMES"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);

            pushInt(x * y, procesID);
            break;

        case DIVIDEDBY:
            Serial.println(F("Executing DIVIDEDBY"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);

            pushInt(x / y, procesID);
            break;

        case MODULUS:
            Serial.println(F("Executing MODULUS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);

            pushInt(x % y, procesID);
            break;

        case UNARYMINUS:
            Serial.println(F("Executing UNARYMINUS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            pushInt(-x, procesID);
            break;

        case EQUALS:
            Serial.println(F("Executing EQUALS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x == y) ? '1' : '0', procesID);
            break;

        case NOTEQUALS:
            Serial.println(F("Executing NOTEQUALS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x != y) ? '1' : '0', procesID);

            break;

        case LESSTHAN:
            Serial.println(F("Executing LESSTHAN"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x < y) ? '1' : '0', procesID);
            break;

        case LESSTHANOREQUALS:
            Serial.println(F("Executing LESSTHANOREQUALS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x <= y) ? '1' : '0', procesID);
            break;

        case GREATERTHAN:
            Serial.println(F("Executing GREATERTHAN"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x > y) ? '1' : '0', procesID);
            break;

        case GREATERTHANOREQUALS:
            Serial.println(F("Executing GREATERTHANOREQUALS"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x >= y) ? '1' : '0', procesID);
            break;

        case LOGICALAND:
            Serial.println(F("Executing LOGICALAND"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x && y) ? '1' : '0', procesID);
            break;

        case LOGICALOR:
            Serial.println(F("Executing LOGICALOR"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar((x || y) ? '1' : '0', procesID);
            break;

        case LOGICALXOR:
            Serial.println(F("Executing LOGICALXOR"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            popByte(procesID); // get type;
            y = popInt(procesID);
            pushChar(((x && !y) || (!x && y)) ? '1' : '0', procesID);
            break;

        case LOGICALNOT:
            Serial.println(F("Executing LOGICALNOT"));
            popByte(procesID); // get type;
            x = popInt(procesID);

            pushChar((!x) ? '1' : '0', procesID);
            break;

        case BITWISEAND:
            Serial.println(F("Executing BITWISEAND"));
            // get type;
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                x = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
            }

            // get type;
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                y = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                y = popInt(procesID);
            }
            pushInt(x & y, procesID);
            break;

        case BITWISEOR:
            Serial.println(F("Executing BITWISEOR"));
            // get type;
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                x = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
            }

            // get type;
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                y = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                y = popInt(procesID);
            }

            pushInt(x | y, procesID);
            break;

        case BITWISEXOR:
            Serial.println(F("Executing BITWISEXOR"));
            // get type;
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                x = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
            }

            // get type;
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                y = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                y = popInt(procesID);
            }

            pushInt(x ^ y, procesID);
            break;

        case BITWISENOT:
            Serial.println(F("Executing BITWISENOT"));
            popType = popByte(procesID);
            if (popType == 'C')
            {
                char charBuf[2];
                charBuf[0] = popChar(procesID);
                charBuf[1] = '\0';
                x = atoi(charBuf);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
            }

            pushInt(~x, procesID);
            break;

        case TOCHAR:
            Serial.println(F("Executing TOCHAR"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                charX = (char)(int)popFloat(procesID);
            }
            else if (popType == 'I')
            {
                charX = (char)popInt(procesID);
            }
            popChar(charX);
            break;

        case TOINT:
            Serial.println(F("Executing TOINT"));
            popType = popByte(procesID);
            if (popType == 'C')
            {
                x = (int)popChar(procesID);
            }
            else if (popType == 'F')
            {
                x = (int)popFloat(procesID);
            }
            pushInt(x, procesID);
            break;

        case TOFLOAT:
            Serial.println(F("Executing TOFLOAT"));
            popType = popByte(procesID);
            if (popType == 'C')
            {
                floatX = (float)popChar(procesID) - '0';
            }
            else if (popType == 'I')
            {
                floatX = (int)popFloat(procesID);
            }
            pushInt(x, procesID);
            break;

        case ROUND:
            Serial.println(F("Executing ROUND"));
            x = popNumber(procesID);
            pushInt(x, procesID);
            break;

        case FLOOR:
            Serial.println(F("Executing FLOOR"));
            x = popNumber(procesID);
            pushInt(x, procesID);
            break;

        case CEIL:
            Serial.println(F("Executing CEIL"));
            popType = popByte(procesID);
            x = popNumber(procesID);

            pushInt(x, procesID);
            break;

        case MIN:
            Serial.println(F("Executing MIN"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);

                popByte(procesID); // get type;
                floatY = popFloat(procesID);

                pushFloat(min(floatX, floatY), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);

                popByte(procesID); // get type;
                y = popInt(procesID);
                pushInt(min(x, y), procesID);
            }

            break;

        case MAX:
            Serial.println(F("Executing MAX"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);

                popByte(procesID); // get type;
                floatY = popFloat(procesID);

                pushFloat(max(floatX, floatY), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);

                popByte(procesID); // get type;
                y = popInt(procesID);
                pushInt(max(x, y), procesID);
            }
            break;

        case ABS:
            Serial.println(F("Executing ABS"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);
                pushFloat(abs(floatX), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
                pushInt(abs(x), procesID);
            }

            break;

        case CONSTRAIN:
            Serial.println(F("Executing CONSTRAIN"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);

                popByte(procesID); // get type;
                floatY = popFloat(procesID);

                popByte(procesID); // get type;
                floatZ = popFloat(procesID);

                pushFloat(constrain(floatX, floatY, floatZ), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);

                popByte(procesID); // get type;
                y = popInt(procesID);

                popByte(procesID); // get type;
                z = popInt(procesID);

                pushInt(constrain(x, y, z), procesID);
            }

            break;

        case MAP:
            Serial.println(F("Executing MAP"));
            // get type;
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);

                popByte(procesID); // get type;
                floatY = popFloat(procesID);

                popByte(procesID); // get type;
                floatZ = popFloat(procesID);

                popByte(procesID); // get type;
                floatA = popFloat(procesID);

                popByte(procesID); // get type;
                floatB = popFloat(procesID);

                pushFloat((floatX - floatY) * (floatB - floatA) / (floatZ - floatY) + floatB, procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);

                popByte(procesID); // get type;
                y = popInt(procesID);

                popByte(procesID); // get type;
                z = popInt(procesID);

                popByte(procesID); // get type;
                a = popInt(procesID);

                popByte(procesID); // get type;
                b = popInt(procesID);

                pushInt((x - y) * (b - a) / (z - y) + b, procesID);
            }

            break;

        case POW:
            Serial.println(F("Executing POW"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);

                popByte(procesID); // get type;
                floatY = popFloat(procesID);

                pushFloat(pow(floatX, floatY), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);

                popByte(procesID); // get type;
                y = popInt(procesID);

                pushInt(pow(x, y), procesID);
            }
            break;

        case SQ:
            Serial.println(F("Executing SQ"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);
                pushFloat(pow(floatX, 2), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
                pushInt(pow(x, 2), procesID);
            }
            break;

        case SQRT:
            Serial.println(F("Executing SQRT"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                floatX = popFloat(procesID);
                pushFloat(sqrt(floatX), procesID);
            }
            else if (popType == 'I')
            {
                x = popInt(procesID);
                pushInt(sqrt(x), procesID);
            }
            break;

        case DELAY:
            Serial.println(F("Executing DELAY"));
            floatX = popVal(procesID);
            delay(floatX);
            break;

        case DELAYUNTIL:
            Serial.println(F("Executing DELAYUNTIL"));
            floatX = popVal(procesID);
            if (floatX > (int)millis())
            {
                procesTable[i].programCounter--;
                pushFloat(floatX, procesID);
            }
            break;

        case MILLIS:
            Serial.println(F("Executing MILLIS"));
            pushInt((int)millis(), procesID);
            break;

        case PINMODE:
            Serial.println(F("Executing PINMODE"));
            floatX = popVal(procesID);
            floatY = popVal(procesID);
            pinMode(floatX, floatY);
            break;

        case ANALOGREAD:
            Serial.println(F("Executing ANALOGREAD"));
            floatX = popVal(procesID);
            analogRead(floatX);
            break;

        case ANALOGWRITE:
            Serial.println(F("Executing ANALOGWRITE"));
            floatX = popVal(procesID);
            floatY = popVal(procesID);
            analogWrite(floatX, floatY);
            break;

        case DIGITALREAD:
            Serial.println(F("Executing DIGITALREAD"));
            floatX = popVal(procesID);
            digitalRead(floatX);
            break;

        case DIGITALWRITE:
            Serial.println(F("Executing DIGITALWRITE"));
            floatX = popVal(procesID);
            floatY = popVal(procesID);
            digitalWrite(floatX, floatY);
            break;

        case PRINT:
            Serial.println(F("Executing PRINT"));
            popType = popByte(procesID);
            if (popType == 'F')
            {
                Serial.print(popFloat(procesID));
            }
            else if (popType == 'I')
            {
                Serial.print(popInt(procesID));
            }
            else if (popType == 'C')
            {
                Serial.print(popChar(procesID));
            }
            else if (popType == 'S')
            {
                Serial.print(popString(procesID));
            }
            break;

        case PRINTLN:
            Serial.println(F("Executing PRINTLN"));
            popType = popByte(procesID);
            if (popType == 'F')
            {

                Serial.println(popFloat(procesID));
            }
            else if (popType == 'I')
            {
                Serial.println(popInt(procesID));
            }
            else if (popType == 'C')
            {
                Serial.println(popChar(procesID));
            }
            else if (popType == 'S')
            {
                Serial.println(popString(procesID));
            }
            break;

        case OPEN:
            Serial.println(F("Executing OPEN"));
            procesTable[i].filePointer = beginAdres;
            break;

        case CLOSE:
            Serial.println(F("Executing CLOSE"));
            break;

        case WRITE:
            Serial.println(F("Executing WRITE"));
            popType = popByte(procesID);
            int addr = procesTable[i].filePointer;

            if (popType == 'F')
            {
                float val = popFloat(procesID);
                EEPROM.put(addr, val);
                procesTable[i].filePointer += sizeof(float);
            }
            else if (popType == 'I')
            {
                int val = popInt(procesID);
                EEPROM.put(addr, val);
                procesTable[i].filePointer += sizeof(int);
            }
            else if (popType == 'C')
            {
                char val = popChar(procesID);
                EEPROM.write(addr, val);
                procesTable[i].filePointer += 1;
            }
            else if (popType == 'S')
            {
                char *str = popString(procesID);
                int len = strlen(str);
                for (int j = 0; j < len; j++)
                {
                    EEPROM.write(addr + j, str[j]);
                }
                procesTable[i].filePointer += len;
            }
            break;

        case READINT:
            Serial.println(F("Executing READINT"));
            int result;
            EEPROM.get(procesTable[i].filePointer, result);
            procesTable[i].filePointer += sizeof(int);
            break;

        case READCHAR:
            Serial.println(F("Executing READCHAR"));
            char result;
            EEPROM.get(procesTable[i].filePointer, result);
            procesTable[i].filePointer += sizeof(char);
            break;

        case READFLOAT:
            Serial.println(F("Executing READFLOAT"));
            float result;
            EEPROM.get(procesTable[i].filePointer, result);
            procesTable[i].filePointer += sizeof(float);
            break;

        case READSTRING:
            Serial.println(F("Executing READSTRING"));
            String result;
            int addr = procesTable[i].filePointer;
            char singleChar;
            int j = 0;
            while (singleChar != '\0')
            {
                result[j] = EEPROM.read(addr + j);
                j++;
            }
            procesTable[i].filePointer += j;
            break;

        case IF:
            Serial.println(F("Executing IF"));
            break;

        case ELSE:
            Serial.println(F("Executing ELSE"));
            break;

        case ENDIF:
            Serial.println(F("Executing ENDIF"));
            break;

        case WHILE:
            Serial.println(F("Executing WHILE"));
            break;

        case ENDWHILE:
            Serial.println(F("Executing ENDWHILE"));
            break;

        case LOOP:
            Serial.println(F("Executing LOOP"));
            break;

        case ENDLOOP:
            Serial.println(F("Executing ENDLOOP"));
            break;

        case STOP:
            Serial.println(F("Executing STOP"));
            break;

        case FORK:
            Serial.println(F("Executing FORK"));
            break;

        case WAITUNTILDONE:
            Serial.println(F("Executing WAITUNTILDONE"));
            break;

        default:
            Serial.print(F("Unknown opcode: "));
            Serial.println(instruction);
            break;
        }
        // kill process when at the end of its life :(
        // Serial.println(name);
        // Serial.println(length);
        // Serial.println(procesCounter);
        if (beginAdres + length <= procesCounter)
        {
            char idStr[2];
            itoa(i, idStr, 2);
            kill(idStr);
            return;
        }
        procesTable[i].programCounter++;
        // Serial.println(i);
    }
}
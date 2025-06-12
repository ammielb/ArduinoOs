#include "procesHandeling.h"
#include "memoryHandeling.h"
#include "EEPROMHandeling.h"
#include "instruction_set.h"

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
    int fileIndex = findName(arg[0]);
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
    procesTable[noOfProces].state = (byte) "r";
    procesTable[noOfProces].programCounter = file.position;
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
    int procesIndex = findID((int)arg[0]);
    if (procesIndex == -1)
    {
        Serial.println(F("Given process ID does not exist"));
        return;
    }

    if (procesTable[procesIndex].state == (byte) "0")
    {
        Serial.println(F("Proces already terminated"));
        return;
    }

    changeState(procesIndex, (byte) "p");
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

    if (procesTable[procesIndex].state == (byte) "0")
    {
        Serial.println(F("Proces already terminated"));
        return;
    }

    changeState(procesIndex, (byte) "r");
}

void kill(const char *arg)
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

    changeState(procesIndex, (byte) "0");
    deleteProcesVars(procesIndex);
    // removeProcesFromList(procesID);
}
void list(const char *arg)
{
    for (size_t i = 0; i < noOfProces; i++)
    {
        if (procesTable[i].state == (byte) "0")
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
}
//
// running processes and executing instructions
//
void runProcesses()
{

    for (size_t i = 0; i < noOfProces - 1; i++)
    {

        execute(i);
    }
}
void execute(int i)
{
    int procesCounter = procesTable[i].programCounter;
    byte instruction = EEPROM.read(procesCounter);
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
        break;

    case GET:
        Serial.println(F("Executing GET"));
        break;

    case INCREMENT:
        Serial.println(F("Executing INCREMENT"));
        break;

    case DECREMENT:
        Serial.println(F("Executing DECREMENT"));
        break;

    case PLUS:
        Serial.println(F("Executing PLUS"));
        break;

    case MINUS:
        Serial.println(F("Executing MINUS"));
        break;

    case TIMES:
        Serial.println(F("Executing TIMES"));
        break;

    case DIVIDEDBY:
        Serial.println(F("Executing DIVIDEDBY"));
        break;

    case MODULUS:
        Serial.println(F("Executing MODULUS"));
        break;

    case UNARYMINUS:
        Serial.println(F("Executing UNARYMINUS"));
        break;

    case EQUALS:
        Serial.println(F("Executing EQUALS"));
        break;

    case NOTEQUALS:
        Serial.println(F("Executing NOTEQUALS"));
        break;

    case LESSTHAN:
        Serial.println(F("Executing LESSTHAN"));
        break;

    case LESSTHANOREQUALS:
        Serial.println(F("Executing LESSTHANOREQUALS"));
        break;

    case GREATERTHAN:
        Serial.println(F("Executing GREATERTHAN"));
        break;

    case GREATERTHANOREQUALS:
        Serial.println(F("Executing GREATERTHANOREQUALS"));
        break;

    case LOGICALAND:
        Serial.println(F("Executing LOGICALAND"));
        break;

    case LOGICALOR:
        Serial.println(F("Executing LOGICALOR"));
        break;

    case LOGICALXOR:
        Serial.println(F("Executing LOGICALXOR"));
        break;

    case LOGICALNOT:
        Serial.println(F("Executing LOGICALNOT"));
        break;

    case BITWISEAND:
        Serial.println(F("Executing BITWISEAND"));
        break;

    case BITWISEOR:
        Serial.println(F("Executing BITWISEOR"));
        break;

    case BITWISEXOR:
        Serial.println(F("Executing BITWISEXOR"));
        break;

    case BITWISENOT:
        Serial.println(F("Executing BITWISENOT"));
        break;

    case TOCHAR:
        Serial.println(F("Executing TOCHAR"));
        break;

    case TOINT:
        Serial.println(F("Executing TOINT"));
        break;

    case TOFLOAT:
        Serial.println(F("Executing TOFLOAT"));
        break;

    case ROUND:
        Serial.println(F("Executing ROUND"));
        break;

    case FLOOR:
        Serial.println(F("Executing FLOOR"));
        break;

    case CEIL:
        Serial.println(F("Executing CEIL"));
        break;

    case MIN:
        Serial.println(F("Executing MIN"));
        break;

    case MAX:
        Serial.println(F("Executing MAX"));
        break;

    case ABS:
        Serial.println(F("Executing ABS"));
        break;

    case CONSTRAIN:
        Serial.println(F("Executing CONSTRAIN"));
        break;

    case MAP:
        Serial.println(F("Executing MAP"));
        break;

    case POW:
        Serial.println(F("Executing POW"));
        break;

    case SQ:
        Serial.println(F("Executing SQ"));
        break;

    case SQRT:
        Serial.println(F("Executing SQRT"));
        break;

    case DELAY:
        Serial.println(F("Executing DELAY"));
        break;

    case DELAYUNTIL:
        Serial.println(F("Executing DELAYUNTIL"));
        break;

    case MILLIS:
        Serial.println(F("Executing MILLIS"));
        break;

    case PINMODE:
        Serial.println(F("Executing PINMODE"));
        break;

    case ANALOGREAD:
        Serial.println(F("Executing ANALOGREAD"));
        break;

    case ANALOGWRITE:
        Serial.println(F("Executing ANALOGWRITE"));
        break;

    case DIGITALREAD:
        Serial.println(F("Executing DIGITALREAD"));
        break;

    case DIGITALWRITE:
        Serial.println(F("Executing DIGITALWRITE"));
        break;

    case PRINT:
        Serial.println(F("Executing PRINT"));
        break;

    case PRINTLN:
        Serial.println(F("Executing PRINTLN"));
        break;

    case OPEN:
        Serial.println(F("Executing OPEN"));
        break;

    case CLOSE:
        Serial.println(F("Executing CLOSE"));
        break;

    case WRITE:
        Serial.println(F("Executing WRITE"));
        break;

    case READINT:
        Serial.println(F("Executing READINT"));
        break;

    case READCHAR:
        Serial.println(F("Executing READCHAR"));
        break;

    case READFLOAT:
        Serial.println(F("Executing READFLOAT"));
        break;

    case READSTRING:
        Serial.println(F("Executing READSTRING"));
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
}
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
    // switch (instruction)
    // {
    // case CHAR:
    //     Serial.println("Executing CHAR");
    //     break;
    // case INT:
    //     Serial.println("Executing INT");
    //     break;
    // case STRING:
    //     Serial.println("Executing STRING");
    //     break;
    // case FLOAT:
    //     Serial.println("Executing FLOAT");
    //     break;
    // case SET:
    //     Serial.println("Executing SET");
    //     break;
    // case GET:
    //     Serial.println("Executing GET");
    //     break;
    // case INCREMENT:
    //     Serial.println("Executing INCREMENT");
    //     break;
    // case DECREMENT:
    //     Serial.println("Executing DECREMENT");
    //     break;
    // case PLUS:
    //     Serial.println("Executing PLUS");
    //     break;
    // case MINUS:
    //     Serial.println("Executing MINUS");
    //     break;
    // case TIMES:
    //     Serial.println("Executing TIMES");
    //     break;
    // case DIVIDEDBY:
    //     Serial.println("Executing DIVIDEDBY");
    //     break;
    // case MODULUS:
    //     Serial.println("Executing MODULUS");
    //     break;
    // case UNARYMINUS:
    //     Serial.println("Executing UNARYMINUS");
    //     break;
    // case EQUALS:
    //     Serial.println("Executing EQUALS");
    //     break;
    // case NOTEQUALS:
    //     Serial.println("Executing NOTEQUALS");
    //     break;
    // case LESSTHAN:
    //     Serial.println("Executing LESSTHAN");
    //     break;
    // case LESSTHANOREQUALS:
    //     Serial.println("Executing LESSTHANOREQUALS");
    //     break;
    // case GREATERTHAN:
    //     Serial.println("Executing GREATERTHAN");
    //     break;
    // case GREATERTHANOREQUALS:
    //     Serial.println("Executing GREATERTHANOREQUALS");
    //     break;
    // case LOGICALAND:
    //     Serial.println("Executing LOGICALAND");
    //     break;
    // case LOGICALOR:
    //     Serial.println("Executing LOGICALOR");
    //     break;
    // case LOGICALXOR:
    //     Serial.println("Executing LOGICALXOR");
    //     break;
    // case LOGICALNOT:
    //     Serial.println("Executing LOGICALNOT");
    //     break;
    // case BITWISEAND:
    //     Serial.println("Executing BITWISEAND");
    //     break;
    // case BITWISEOR:
    //     Serial.println("Executing BITWISEOR");
    //     break;
    // case BITWISEXOR:
    //     Serial.println("Executing BITWISEXOR");
    //     break;
    // case BITWISENOT:
    //     Serial.println("Executing BITWISENOT");
    //     break;
    // case TOCHAR:
    //     Serial.println("Executing TOCHAR");
    //     break;
    // case TOINT:
    //     Serial.println("Executing TOINT");
    //     break;
    // case TOFLOAT:
    //     Serial.println("Executing TOFLOAT");
    //     break;
    // case ROUND:
    //     Serial.println("Executing ROUND");
    //     break;
    // case FLOOR:
    //     Serial.println("Executing FLOOR");
    //     break;
    // case CEIL:
    //     Serial.println("Executing CEIL");
    //     break;
    // case MIN:
    //     Serial.println("Executing MIN");
    //     break;
    // case MAX:
    //     Serial.println("Executing MAX");
    //     break;
    // case ABS:
    //     Serial.println("Executing ABS");
    //     break;
    // case CONSTRAIN:
    //     Serial.println("Executing CONSTRAIN");
    //     break;
    // case MAP:
    //     Serial.println("Executing MAP");
    //     break;
    // case POW:
    //     Serial.println("Executing POW");
    //     break;
    // case SQ:
    //     Serial.println("Executing SQ");
    //     break;
    // case SQRT:
    //     Serial.println("Executing SQRT");
    //     break;
    // case DELAY:
    //     Serial.println("Executing DELAY");
    //     break;
    // case DELAYUNTIL:
    //     Serial.println("Executing DELAYUNTIL");
    //     break;
    // case MILLIS:
    //     Serial.println("Executing MILLIS");
    //     break;
    // case PINMODE:
    //     Serial.println("Executing PINMODE");
    //     break;
    // case ANALOGREAD:
    //     Serial.println("Executing ANALOGREAD");
    //     break;
    // case ANALOGWRITE:
    //     Serial.println("Executing ANALOGWRITE");
    //     break;
    // case DIGITALREAD:
    //     Serial.println("Executing DIGITALREAD");
    //     break;
    // case DIGITALWRITE:
    //     Serial.println("Executing DIGITALWRITE");
    //     break;
    // case PRINT:
    //     Serial.println("Executing PRINT");
    //     break;
    // case PRINTLN:
    //     Serial.println("Executing PRINTLN");
    //     break;
    // case OPEN:
    //     Serial.println("Executing OPEN");
    //     break;
    // case CLOSE:
    //     Serial.println("Executing CLOSE");
    //     break;
    // case WRITE:
    //     Serial.println("Executing WRITE");
    //     break;
    // case READINT:
    //     Serial.println("Executing READINT");
    //     break;
    // case READCHAR:
    //     Serial.println("Executing READCHAR");
    //     break;
    // case READFLOAT:
    //     Serial.println("Executing READFLOAT");
    //     break;
    // case READSTRING:
    //     Serial.println("Executing READSTRING");
    //     break;
    // case IF:
    //     Serial.println("Executing IF");
    //     break;
    // case ELSE:
    //     Serial.println("Executing ELSE");
    //     break;
    // case ENDIF:
    //     Serial.println("Executing ENDIF");
    //     break;
    // case WHILE:
    //     Serial.println("Executing WHILE");
    //     break;
    // case ENDWHILE:
    //     Serial.println("Executing ENDWHILE");
    //     break;
    // case LOOP:
    //     Serial.println("Executing LOOP");
    //     break;
    // case ENDLOOP:
    //     Serial.println("Executing ENDLOOP");
    //     break;
    // case STOP:
    //     Serial.println("Executing STOP");
    //     break;
    // case FORK:
    //     Serial.println("Executing FORK");
    //     break;
    // case WAITUNTILDONE:
    //     Serial.println("Executing WAITUNTILDONE");
    //     break;
    // default:
    //     Serial.print("Unknown opcode: ");
    //     Serial.println(instruction);
    //     break;
    // }
}
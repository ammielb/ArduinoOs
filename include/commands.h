#ifndef COMMANDS_H
#define COMMANDS_H
#define fileDataBegin = 160

// Prototype functions
void store(const char *arg);
void erase(const char *arg);
void retrieve(const char *arg);
void files(const char *arg);
void freespace(const char *arg);
void showEEPROM(const char *arg);

void unknownCommand(const char *arg);
void commandHandler(const char **userInput);

#define BUFSIZE 12
typedef void (*CommandFunc)(const char *arg);

typedef struct
{
    char name[BUFSIZE];
    CommandFunc func;
} commandType;

// Lijst met beschikbare commando's

#endif
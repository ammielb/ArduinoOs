#include <Arduino.h>
#include <CLIHandeling.h>
#include <string.h>
static byte inputIndex = 0;

// Functie om een token (woord) in te lezen
bool readToken(char *buffer)
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == ' ')
        {
            if (inputIndex == 0)
                continue;
            buffer[inputIndex++] = ' ';
        }
        else if (c == '\n' || c == '\r')
        {
            if (inputIndex == 0)
                continue; // negeer dubbele spaties
            buffer[inputIndex] = '\0';
            inputIndex = 0;
            return true;
        }
        else
        {
            if (inputIndex < BUFSIZE - 1)
            {
                buffer[inputIndex++] = c;
            }
        }
    }
    return false;
}

char *parts[2];
char **seperateArgCommand(char *input)
{
    parts[0] = strtok(input, " ");
    parts[1] = strtok(NULL, "");
    return parts;
}
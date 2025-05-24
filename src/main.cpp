#include <Arduino.h>
#include <commands.h>
#include <CLIHandeling.h>
#include <EEPROMHandeling.h>

static char token[BUFSIZE];

// fileInfo file1 = {"name1", 161, "123123123"};
// fileInfo file2 = {"name2", 12, "asdasdas"};
// fileInfo file3 = {"name3", 12, 5};
// const int nCommands = sizeof(commands) / sizeof(commandType);
void setup()
{
  Serial.begin(9600);
  Serial.println("ArduinOS 1.0 ready.");
  Serial.print("> ");
  for (int i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.write(i, 0);
  }
  noOfFiles = 0;
  store("file1 12 bbbbbbb");
  store("file2 12 aaaaaaaa");
  store("file3 12 ccccccccc");
}
int argCount = 0;

void loop()
{

  if (readToken(token))
  {
    char **userInput = seperateArgCommand(token);
    commandHandler(userInput);
  }
}
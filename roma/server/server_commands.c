#include "server_commands.h"
#include "client.h"
#include <string.h>
#include "../hw/led.h"

void ProcessCommand(char *command)
{
    if (CompareStrings(command, ":pene") == 0)
        Pene();
    else if (CompareStrings(command, ":green") == 0)
        GreenLight();
    else if (CompareStrings(command, ":red") == 0)
        RedLight();
    else if (CompareStrings(command, ":yellow") == 0)
        YellowLight();
    else if (CompareStrings(command, ":blue") == 0)
        BlueLight();
    else if (CompareStrings(command, ":hohoho") == 0)
        RandomLights();
    else
        Help();
}

int CompareStrings(char *a, char *b)
{
    return strncmp(a, b, strlen(a));
}

void Pene()
{
    Log_msg("PENEPE");
};

void GreenLight()
{
    Log_msg("GREEN LIGHT");
    ToggleGreenLight();
}

void RedLight()
{
    Log_msg("RED LIGHT");
    ToggleRedLight();
}

void YellowLight()
{
    Log_msg("YELLOW LIGHT");
    ToggleYellowLight();
}

void BlueLight()
{
    Log_msg("BLUE LIGHT");
    ToggleBlueLight();
}

void RandomLights()
{
    Log_msg("RANDOM LIGHTS");
    ToggleChristmas();
}
void Help()
{
    Log_msg("Available commands:\npene\ngreenlight\nredlight\nbluelight\nyellowlight\nhohoho");
}
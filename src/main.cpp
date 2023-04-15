#include <iostream>
#include <conio.h>
#include <mosquitto.h>

#include "Server.h"
#include "Alphabot.h"
#include "Graffiti.h"
#include "ImageProcessing.h"
#include "KeyParser.h"

int main()
{
    Server server;
    KeyParser keyParser(&server);
    std::cout << "Started!" << std::endl;

    // Считываем настройки из файла
    server.readSettings();

    while (true)
    {
        if (!keyParser.parseKeys())
        {
            break;
        }
    }
    return 0;
}
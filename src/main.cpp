#include <iostream>

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

    // запускаем mqtt python
    system("start cmd /c python ../../src/mqtt.py");

    while (true)
    {
        if (!keyParser.parseKeys())
        {
            ofstream file("../../docs/message.txt");
            file << "EXIT";
            break;
        }
    }
    return 0;
}
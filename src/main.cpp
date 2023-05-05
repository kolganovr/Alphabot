#include <iostream>
#include <chrono>

#include "Server.h"
#include "Alphabot.h"
#include "Graffiti.h"
#include "ImageProcessing.h"
#include "KeyParser.h"

int main()
{
    system("start cmd /c python ../../src/settings.py");
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
            ofstream file("../../.temp/message.txt");
            file << "EXIT";
            file.close();
            break;
        }
    }

    // Задержка в 1 секунду для завершения работы mqtt
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Удаляем временные файлы
    remove("../../.temp/message.txt");
    remove("../../.temp/config.txt");
    remove("../../.temp/hsvValues.txt");

    return 0;
}
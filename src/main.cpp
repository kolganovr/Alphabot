#include <iostream>

#include "Server.h"
#include "Alphabot.h"
#include "Graffiti.h"
#include "ImageProcessing.h"
#include "KeyParser.h"

int main()
{
    cout << "Turn off VPN and firewall!" << endl;
    system("python ../../src/settings.py");
    Server server;
    KeyParser keyParser(&server);
    std::cout << "Started!" << std::endl;

    // Считываем настройки из файла
    server.readSettings();
    std::cout << "Settings read!" << std::endl;

    // Получаем путь к файлу с конфигурацией на диске
    string pathToMosquittoConf = server.getConfPath();
    std::cout << "Path to mosquitto config: " << pathToMosquittoConf << std::endl; 

    // Запускаем mqtt брокер
    system(("start cmd as admin /c mosquitto -c " + pathToMosquittoConf + " -v").c_str());

    while (true)
    {
        if (!keyParser.parseKeys())
        {
            break;
        }
    }

    // Удаляем временные файлы
    remove("../../.temp/config.txt");
    remove("../../.temp/hsvValues.txt");
    return 0;
}
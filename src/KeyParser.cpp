#include "KeyParser.h"

KeyParser::KeyParser(Server *server)
{
    this->server = server;
}

bool KeyParser::parseKeys()
{
    char key = waitKey(1);

    // Если нажат пробел, то переключаем режим отладки
    if (key == keys.space)
    {
        debugMode = !debugMode;
        server->setDebugMode(debugMode);
        cout << "Debug mode: " << debugMode << endl;
    }

    server->receiveMessage();

    if (debugMode)
    {
        server->sendImageToThresholdGenerator();

        // Если нажата клавиша, то сохраняем настройки пороговых значений для заданного цвета
        if (key == keys.one)
        {
            server->setHSV("purple");
        }
        else if (key == keys.two)
        {
            server->setHSV("green");
        }
        else if (key == keys.three)
        {
            server->setHSV("blue");
        }
        else if (key == keys.s)
        {
            server->sendHSVToThresholdGenerator();
        }
        else if (key == keys.l)
        {
            server->recieveHSVFromThresholdGenerator();
        }
    }
    else
    {
        server->alphabotRun();
    }

    if (key == keys.esc)
    {
        return false; // Выходим из программы
    }

    return true; // Продолжаем работу
}
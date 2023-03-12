#include <iostream>

#include "Server.h"
#include "Alphabot.h"
#include "Graffiti.h"
#include "ImageProcessing.h"

using namespace cv;
using namespace std;

int main()
{
    bool isDebug = false;
    Server server;

    while (true)
    {
        // Получаем нажатую клавишу
        char key = waitKey(1);

        // Если нажат пробел, то переключаем режим отладки
        if (key == 32)
        {
            isDebug = !isDebug;
            server.setDebugMode(isDebug);
            cout << "Debug mode: " << isDebug << endl;
        }

        // Получаем сообщение от камеры
        server.receiveMessage();

        if (isDebug)
        {
            server.sendImageToThresholdGenerator();

            // Если нажата клавиша, то сохраняем настройки пороговых значений для заданного цвета
            if (key == 49) // 1
            {
                server.setHSV("purple");
            }
            else if (key == 50) // 2
            {
                server.setHSV("green");
            }
            else if (key == 51) // 3
            {
                server.setHSV("blue");
            }
            else if (key == 115) // s
            {
                server.sendHSVToThresholdGenerator();
            }
            else if (key == 108) // l
            {
                server.recieveHSVFromThresholdGenerator();
            }
        }
        else
        {
            // Опрашиваем робота
            server.alphabotRun();
        }

        // Если нажата ESC, то выходим из программы
        if (key == 27)
        {
            break;
        }
    }
    return 0;
}
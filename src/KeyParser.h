#pragma once
#include "Server.h"

// Класс обработки нажатий клавиш
class KeyParser
{
private:
    Server *server;
    bool debugMode = false; // Режим отладки

    // Структура для хранения клавиш
    struct keys
    {
        const char space = 32; // Пробел
        const char one = 49;   // 1
        const char two = 50;   // 2
        const char three = 51; // 3
        const char s = 115;    // s
        const char s_ru = -5;  // ы
        const char l = 108;    // l
        const int l_ru = -28;  // д
        const char esc = 27;   // escape
        const char c = 99;     // c
        const int c_ru = -15;  // с
    } keys;

public:
    KeyParser(Server *server);

    /// Обработчик нажатий клавиш
    /// @return true - продолжаем работу, false - выходим из программы
    bool parseKeys();
};
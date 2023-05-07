#pragma once
#include <iostream>
#include <string>

using namespace std;

class Mqtt
{
private:
    string brokerIP = "";     // IP брокера
    string topic = "";        // Топик
    string prevMessage = "";  // Предыдущее сообщение
    string pathToConfig = ""; // Путь к конфигурационному файлу

    /// Создание команды для отправки на брокер
    /// @param command Команда
    /// @param time Время выполнения команды
    /// @return Команда для отправки на брокер
    string makeCommand(string message, string time);

    // Получение пути к конфигурационному файлу
    void findPathToConfig();

public:
    Mqtt();

    ~Mqtt();

    /// Получение пути к конфигурационному файлу
    /// @return Путь к конфигурационному файлу
    string getPathToConfig();

    /// Настройка брокера
    /// @param brokerIP IP брокера
    /// @param topic Топик
    void setupBroker(string brokerIP, string topic);

    /// Отправка команды на брокер
    /// @param command Команда
    /// @param time Время выполнения команды
    void publish(string command, string time);
};
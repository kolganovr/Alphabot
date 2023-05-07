#include "mqtt.h"
#include <windows.h>
#include <iostream>

using namespace std;

Mqtt::Mqtt()
{
    findPathToConfig();
}

Mqtt::~Mqtt()
{
    // Останавливаем брокер
    system("taskkill /IM mosquitto.exe /F");
}

string Mqtt::makeCommand(string message, string time)
{
    string cmdTemplate = "mosquitto_pub -h " + brokerIP + " -t " + topic + " -m ";
    string command = "{\\\"cmd\":\\\"" + message + "\\\"";

    if (message == "stop")
    {
        if (prevMessage == "stop")
            return "";
        else
            command += "}";
    }
    else
    {
        command += ",\\\"time\\\":" + time + ",\\\"val\\\":1}";
    }
    prevMessage = message;
    return cmdTemplate + command;
}

void Mqtt::findPathToConfig()
{
    // Путь к файлу, для которого нужно получить абсолютный путь
    const char *filename = "mosquittoWeb.conf";

    // Буфер для хранения абсолютного пути
    char path[MAX_PATH];

    // Получение абсолютного пути к файлу
    DWORD result = GetFullPathName(filename, MAX_PATH, path, NULL);

    // Проверка на ошибки
    if (result == 0)
    {
        cout << "Ошибка при получении абсолютного пути: " << GetLastError() << endl;
        return;
    }

        string pathStr = path;
    pathStr.replace(pathStr.find("build\\Release"), 13, "docs");

    pathToConfig = pathStr;
}

string Mqtt::getPathToConfig()
{
    return pathToConfig;
}

void Mqtt::setupBroker(string brokerIP, string topic)
{
    this->brokerIP = brokerIP;
    this->topic = topic;

    // Подписываемся на топик
    string command = "start cmd /c mosquitto_sub -h " + brokerIP + " -t " + topic;
    system(command.c_str());
}

void Mqtt::publish(string message, string time)
{
    if (brokerIP == "" || topic == "")
    {
        cout << "Broker IP or topic is not set!" << endl;
        return;
    }

    string command = makeCommand(message, time);
    if (command != "")
        system(command.c_str());
}
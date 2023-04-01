#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "Graffiti.h"

using namespace cv;

// Действия которые может выполнять робот
enum Action
{
    FORWARD,
    LEFT,
    RIGHT,
    CLEAN,
    IDLE
};

// Класс для управления двигателями
class EngineController
{
public:
    // Выполняет движение вперед заданное время
    void moveForward(int time);
    // Выполняет поворот налево заданное время
    void moveLeft(int time);
    // Выполняет поворот направо заданное время
    void moveRight(int time);
};

// Класс для управления механизмом очистки
class Cleaner
{
public:
    // Выполняет очистку
    void perform();
};

// Класс робота
class Alphabot
{
private:
    EngineController engineController;
    Cleaner cleaner;

    bool engineStarted; // Состояние двигателей
    int posX;           // Текущая X координата
    int posY;           // Текущая Y координата
    Action state;       // Текущее состояние робота

    // Движение робота или очистка
    void doAction(int time);

public:
    Alphabot();

    /// @brief Устанавливает позицию робота
    /// @param x X позиция
    /// @param y Y позиция
    void SetPosition(int x, int y);

    /// Получает X позицию робота
    /// @return X позиция
    int getPosX();

    /// Получает Y позицию робота
    /// @return Y позиция
    int getPosY();

    // Запускает двигатель
    void startEngine();
    
    // Останавливает двигатель
    void stopEngine();

    /// Возвращает состояние двигателя
    /// @return true если двигатель запущен
    bool isEngineStarted();

    /// Устанавливает состояние робота
    /// @param state Состояние робота
    void setState(int state);

    // Опрашивает робота и вызывает выполнение действия
    void run();
};
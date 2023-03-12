#include "Alphabot.h"
#include "Graffiti.h"

using namespace std;

enum Action
{
    FORWARD,
    LEFT,
    RIGHT,
    CLEAN,
    IDLE
};

void EngineController::moveForward(int time)
{
    cout << "moveForward" << endl;
}

void EngineController::moveLeft(int time)
{
    cout << "moveLeft" << endl;
}

void EngineController::moveRight(int time)
{
    cout << "moveRight" << endl;
}

void Cleaner::perform()
{
    cout << "clean" << endl;
}

void Alphabot::doAction(int time)
{
    switch (state)
    {
    case FORWARD:
        engineController.moveForward(time);
        break;
    case LEFT:
        engineController.moveLeft(time);
        break;
    case RIGHT:
        engineController.moveRight(time);
        break;
    case CLEAN:
        cleaner.perform();
        break;
    case IDLE:
        break;
    default:
        break;
    }
}

Alphabot::Alphabot()
{
    engineStarted = false;
    state = IDLE;
    posX = 0;
    posY = 0;
}

void Alphabot::SetPosition(int x, int y)
{
    posX = x;
    posY = y;
}

int Alphabot::getPosX() { return posX; }

int Alphabot::getPosY() { return posY; }

void Alphabot::startEngine()
{
    cout << "Engine started" << endl;
    engineStarted = true;
}

void Alphabot::stopEngine()
{
    cout << "Engine stopped" << endl;
    engineStarted = false;
}

bool Alphabot::isEngineStarted()
{
    return engineStarted;
}

void Alphabot::setState(int state)
{
    switch (state)
    {
    case 0:
        this->state = FORWARD;
        break;
    case 1:
        this->state = LEFT;
        break;
    case 2:
        this->state = RIGHT;
        break;
    case 3:
        this->state = CLEAN;
        break;
    case 4:
        this->state = IDLE;
        break;
    default:
        break;
    }
}

void Alphabot::run()
{
    // Если двигатель не запущен или робот находится в состоянии IDLE, то ничего не делаем
    if (!engineStarted || state == 4)
    {
        return;
    }
    doAction(1);
}
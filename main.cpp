#include <iostream>

enum Action
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    CLEAN
};

class EngineController
{
public:
    void moveForward(int time);
    void moveBackwards(int time);
    void moveLeft(int time);
    void moveRight(int time);
};

class Cleaner
{
public:
    void perform();
};

class Camera
{   
public:
    void sendToServer();
};

class Graffity{
    bool exist = false;
    int posX;
    int posY;
public:
    Graffity(int x, int y)
    {
        posX = x;
        posY = y;
        exist = true;
    }
    int getPosX() { return posX; }
    int getPosY() { return posY; }
};

class Server
{
private:
    Graffity searchForGraffity(){}
public:
    void receiveMessage();
    void sendMessage(Graffity graffity);
};

class Alphabot
{
private:
    bool engineStarted;
    EngineController engineController;
    Cleaner cleaner;
    int posX;
    int posY;
    Graffity target;

    Action chooseDir(Graffity curTarget) {}
    void doAction(Action action, int time)
    {
        switch (action)
        {
        case FORWARD:
            engineController.moveForward(time);
            break;
        case BACKWARD:
            engineController.moveBackwards(time);
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
        default:
            break;
        }
    }
public:
    void startEngine()
    {
        engineStarted = true;
    }
    void stopEngine()
    {
        engineStarted = false;
    }
};
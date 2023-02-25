#include <iostream>

// This enum represents the actions that the Alphabot can perform.
enum Action
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    CLEAN
};

// This class is used to control the movement of the robot.
class EngineController
{
public:
    // Moves the robot forward for the specified time.
    void moveForward(int time);
    // Moves the robot backwards for the specified time.
    void moveBackwards(int time);
    // Moves the robot left for the specified time.
    void moveLeft(int time);
    // Moves the robot right for the specified time.
    void moveRight(int time);
};

// This class is used to control the cleaning ability of robot.
class Cleaner
{
public:
    // Performs the cleaning action.
    void perform();
};

// Class for getting the image and transfering it to server.
class Camera
{   
public:
    // Sends the image to the server.
    void sendToServer(); 
};

// This class represents a graffiti
// It has a position and a boolean that tells if it exists or not
class Graffiti{
    bool exist = false; // Does graffiti exist
    int posX; // X position of graffiti
    int posY; // Y position of graffiti
public:
    // Constructor
    Graffiti(int x, int y)
    {
        posX = x;
        posY = y;
        exist = true;
    }
    
    /// Returns X position of graffiti
    /// @return position (0 is Left side (example!) )
    int getPosX() { return posX; }
    /// Returns Y position of graffiti
    /// @return position (0 is Down side (example!) )
    int getPosY() { return posY; }
};

class Alphabot
{
private:
    bool engineStarted; 
    EngineController engineController;
    Cleaner cleaner;
    int posX; // Current X coordinate
    int posY; // Current Y coordinate
    Graffiti target; // Current graffiti target

    /// Choosing Actiion based on Graffiti pos 
    /// @param curTarget object we are heading to
    /// @return Chosed Action
    Action chooseAction(Graffiti curTarget);

    // Move robot, or clean graffiti.
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
    /// gets a messsage from the Server
    /// @param target graffity object robot needs to go to
    void recieveMessage(Graffiti graffiti){
        target = graffiti;
    }
    void startEngine()
    {
        engineStarted = true;
    }
    void stopEngine()
    {
        engineStarted = false;
    }
};

// This class represents the server that is used to communicate between the robot and the camera.
class Server
{
private:
    Alphabot alphabot;
    /// Searching for Graffiti positions
    /// @return Graffiti object
    Graffiti searchForGraffiti();
public:
    // Receives messages from the user.
    void receiveMessage();
    // Sends messages to the user.
    void sendMessage(Graffiti graffiti);
};
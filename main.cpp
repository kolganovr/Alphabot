#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <tuple>

using namespace cv;
using namespace std;

#define MIN_CONTOUR_AREA 250
#define M_PI 3.1416

// Действия которые может выполнять робот
enum Action
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    CLEAN
};

// Класс для управления двигателями
class EngineController
{
public:
    // Выполняет движение вперед заданное время
    void moveForward(int time);
    // Выполняет движение назад заданное время
    void moveBackwards(int time);
    // Выполняет поворот налево заданное время
    void moveLeft(int time);
    // Выполняет поворот направо заданное время
    void moveRight(int time);
};

// Класс для управления моющими щетками
class Cleaner
{
public:
    // Выполняет очистку поверхности
    void perform();
};

// Класс для управления камерой
class Camera
{
private:
    // VideoCapture объект для считывания изображения с камеры
    VideoCapture cap;

    /// Получает изображение с камеры
    /// @return Кортеж из двух матриц -- HSV и frame
    Mat getImage()
    {
        // Считываем кадр
        Mat frame;
        cap.read(frame);

        // Переводим кадр в HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Отображаем исходный кадр
        imshow("Original", frame);

        // Возвращаем HSV
        return hsv;
    }

public:
    Camera()
    {
        // Инициализируем камеру
        cap.open(0);
        // Задаём размеры изображения
        cap.set(CAP_PROP_FRAME_WIDTH, 640);
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);

        // Создаем окна для отображения исходного кадра и фильтрованного
        namedWindow("Original", WINDOW_NORMAL);
        // namedWindow("Filtered", WINDOW_NORMAL);
    }

    ~Camera()
    {
        // Закрываем камеру
        cap.release();
        destroyAllWindows();
    }

    /// Отправляем изображение на сервер
    /// @return Кортеж из двух матриц -- HSV и frame
    Mat sendToServer()
    {
        return getImage();
    }
};

// Класс обьъекта граффити
class Graffiti
{
    bool exist = false; // Существование граффити
    int posX;           // X позиция граффити
    int posY;           // Y позиция граффити
public:
    Graffiti() {}
    Graffiti(int x, int y)
    {
        posX = x;
        posY = y;
        exist = true;
    }

    /// Возвращает X позицию граффити
    /// @return Позиция X -- 0 Левая сторона
    int getPosX() { return posX; }
    /// Возвращает Y позицию граффити
    /// @return Позиция Y -- 0 верхняя сторона
    int getPosY() { return posY; }
};

// Класс робота
class Alphabot
{
private:
    bool engineStarted;
    EngineController engineController;
    Cleaner cleaner;
    int posX;        // Текущая X координата
    int posY;        // Текущая Y координата
    Graffiti target; // Текущая позиция цели

    /// Выбирает действие на основании позиции граффити
    /// @param curTarget объект к которому движемся
    /// @return выбранное действие
    Action chooseAction(Graffiti curTarget);

    // Движение робота или очистка
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
    Alphabot() {
        engineStarted = false;
        posX = 0;
        posY = 0;
    }
    /// Получает сообщенеи с сервера
    /// @param target граффити к кторому нужно двигаться
    void recieveMessage(Graffiti graffiti)
    {
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

// Класс сервера для коммуникации между камерой и роботом а также вычислений
class Server
{
private:
    Alphabot alphabot;
    Camera camera;
    Mat hsv;
    // Mat result;

    /// Ищет граффити
    /// @return Graffiti объект граффити
    Graffiti searchForGraffiti();

    void showResults()
    {
        // Отображаем результаты
        imshow("Original", hsv);
        // imshow("Filtered", result);
    }

public:
    Server()
    {
        // Инициализируем камеру
        camera = Camera();
    }
    // Получает сообщение от камеры, отправляет роботу и отображает результаты
    void receiveMessage()
    {
        // Получаем изображение с камеры
        Mat hsv = camera.sendToServer();

        // Ищем граффити
        // Graffiti graffiti = searchForGraffiti();

        // Отображаем результаты
        // showResults();

        // Отправляем сообщение роботу
        // sendMessage(graffiti);
    }
    // Отправляет сообщение роботу
    void sendMessage(Graffiti graffiti)
    {
        alphabot.recieveMessage(graffiti);
    }
};

int main()
{
    Server server;
    Camera camera;
    while (true)
    {
        // Получаем сообщение от камеры
        // server.receiveMessage();
        camera.sendToServer();

        // Ожидаем нажатия клавиши
        // При нажатии на esc выходим
        if (waitKey(1) == 27)
            break;
    }
    return 0;
}
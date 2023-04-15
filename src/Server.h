#pragma once
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>
#include <string>

#include "Alphabot.h"
#include "ImageProcessing.h"
#include "Graffiti.h"

// Класс сервера для коммуникации между камерой и роботом а также вычислений
class Server
{
private:
    Alphabot alphabot;
    Graffiti graffiti;
    Camera camera;
    ThresholdGenerator thresholdGenerator;
    Mat frame;
    Mat hsv;
    Mat robotRes;
    Mat graffitiRes;

    bool debugMode = false;
    double angle = 0; // Угол поворота робота относительно граффити
    bool showGraphics = false; // Показывать графику на изображении

    // Уставнавливаем нижнюю и верхнюю границу цветов маркера
    // Порядок: красный, зеленый, синий
    vector<Scalar> lowerHSV = {Scalar(150, 90, 197), Scalar(50, 116, 40), Scalar(91, 179, 125)};
    // Порядок: красный, зеленый, синий
    vector<Scalar> upperHSV = {Scalar(179, 219, 255), Scalar(93, 222, 147), Scalar(108, 255, 255)};
    
    /// Поиск граффити на изображении
    /// @return координаты граффити
    tuple<int, int> searchForGraffiti();

    /// Вычисляет угол наклона робота
    /// @param puprle_X X координата фиолетовой точки
    /// @param purple_Y Y координата фиолетовой точки
    /// @param blue_X X координата синей точки
    /// @param blue_Y Y координата синей точки
    /// @return угол наклона в градусах
    double AlphabotAngleCalc(int purple_X, int purple_Y, int blue_X, int blue_Y, int target_X, int target_Y);

    /// Ристует графику поверх изображения
    /// @param blue_X X позиция синего маркера
    /// @param blue_Y Y позиция синего маркера
    /// @param purple_X X позиция фиолетового маркера
    /// @param purple_Y Y позиция фиолетового маркера
    /// @param angle угол
    /// @param purpleContour фиолетовый маркер найден
    /// @param blueContour синий маркер найден
    void drawGraphics(int purple_X, int purple_Y, int blue_X, int blue_Y, double angle, bool purpleContour, bool blueContour, Graffiti graffiti);

    /// Ищет позцию робота на кадре и устанавливает его позицию
    /// @param graffiti граффити
    void searchForRobot(Graffiti graffiti);

    // Выбирает действие для робота
    void choseAction();

    /// @brief Отображает исходный кадр и кадр с результатами фильтрации
    void showResults();

public:
    Server();

    // Получает сообщение от камеры, отправляет роботу и отображает результаты
    void receiveMessage();

    // Отправляет изображение с камеры в генератор маски
    void sendImageToThresholdGenerator();

    // Отправляет границы цветов в генератор маски для сохранения в файл
    void sendHSVToThresholdGenerator();

    // Получает границы цветов из генератора маски
    void recieveHSVFromThresholdGenerator();

    /// Устанавливает режим отладки
    /// @param isDebug - true, если включен режим отладки
    void setDebugMode(bool isDebug);

    /// Возвращает режим отображения графики
    /// @return true, если графика отображается
    bool getShowGraphics();

    /// Устанавливает границы цветов
    /// @param color - цвет, границы которого нужно установить (purple, blue, green)
    void setHSV(string color);

    // Запускает опрос робота
    void alphabotRun();

    // Переключает режим камеры
    void setCameraType();

    // Считывает настройки из файла docs/config.txt
    void readSettings();
};
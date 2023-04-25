#pragma once
#include <opencv2/opencv.hpp>
#include <tuple>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

// Класс для управления камерой
class Camera
{
private:
    // VideoCapture объект для считывания изображения с камеры
    VideoCapture cap;

    // Выбор камеры (1 - встроенная, 0 - внешняя)
    bool isWebcam = true;

    // IP адрес внешней камеры
    string IP_CAM = "";

    /// Получает изображение с камеры
    /// @return Кортеж из двух матриц -- HSV и frame
    tuple<Mat, Mat> getImage();

    // Подключаемся к встроеенной камере
    void connectToWebcam();

    // Подключаемся к внешней камере
    void connectToIPcam();

public:
    Camera();

    ~Camera();

    /// Отправляем изображение на сервер
    /// @return Кортеж из двух матриц -- HSV и frame
    tuple<Mat, Mat> sendToServer();

    /// Устанавливает камеру
    /// @param isWebcam true - встроенная, false - внешняя
    void changeCameraType();

    /// Устанавливает IP адрес внешней камеры
    /// @param ip IP адрес внешней камеры
    void setIPcam(string ip);

    /// Возвращает тип камеры
    /// @return true - встроенная, false - внешняя
    bool getCameraType();
};

class ThresholdGenerator
{
private:
    Mat mask;
    Mat hsv;
    Mat frame;

    // Нижние значения HSV для трекбаров
    vector<int> lowerHSV = {0, 0, 0};
    // Верхние значения HSV для трекбаров
    vector<int> upperHSV = {179, 255, 255};

    // Время выполнения команды альфаботом
    int commandTime = 1;

    /// Функция для получения значений маски из файла
    /// @return два вектора значений маски сначала для нижней границы, потом для верхней границы
    /// (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    vector<vector<Scalar>> getHSVfromFile();

public:
    ThresholdGenerator();

    /// Функция для создания трекбаров для настройки фильтра HSV
    void trackBar();

    /// Функция для сохранений значений маски в файл
    /// @param lowerHSV вектор значений нижней границы маски для каждого цвета (hMin, sMin, vMin) в порядке: красный, зеленый, синий
    /// @param upperHSV вектор значений верхней границы маски для каждого цвета (hMax, sMax, vMax) в порядке: красный, зеленый, синий
    void saveHSVtoFile(vector<Scalar> &lowerHSV, vector<Scalar> &upperHSV);

    /// Функция для получения изображения с камеры
    void recieveImage(const Mat &hsv, const Mat &frame);

    /// Функция для отправки маски на сервер
    /// @return вектор значений маски сначала для нижней границы, потом для верхней границы
    /// (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    vector<vector<Scalar>> sendHSVtoServer();

    /// Функция для получения значений с трекбаров
    tuple<int, int, int, int, int, int> getHSV();

    /// Функция для получения времени выполнения команды альфаботом
    /// @return время выполнения команды альфаботом в секундах в виде строки
    string getCommandTime();
};
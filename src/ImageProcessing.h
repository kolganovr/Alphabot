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

    /// Получает изображение с камеры
    /// @return Кортеж из двух матриц -- HSV и frame
    tuple<Mat, Mat> getImage();

public:
    Camera();

    ~Camera();

    /// Отправляем изображение на сервер
    /// @return Кортеж из двух матриц -- HSV и frame
    tuple<Mat, Mat> sendToServer();
};

class ThresholdGenerator
{
private:
    Mat mask;
    Mat hsv;
    Mat frame;

    int hMin = 0, hMax = 179, sMin = 0, sMax = 255, vMin = 0, vMax = 255; // Переменные для настройки фильтра HSV

public:
    ThresholdGenerator();

    /// Функция для создания трекбаров для настройки фильтра HSV
    void trackBar();

    /// Функция для сохранений значений маски в файл
    /// @param hsvValues вектор значений маски для каждого цвета (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    void saveHSVtoFile(vector<Scalar> hsvValues);

    /// Функция для получения значений маски из файла
    /// @return вектор значений маски для каждого цвета (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    tuple<Scalar, Scalar, Scalar, Scalar, Scalar, Scalar> getHSVfromFile();

    /// Функция для получения изображения с камеры
    void recieveImage(const Mat &hsv, const Mat &frame);

    /// Функция для отправки маски на сервер
    /// @return параметры маски (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    tuple<Scalar, Scalar, Scalar, Scalar, Scalar, Scalar> sendHSVtoServer();

    /// Функция для получения значений с трекбаров
    tuple<int, int, int, int, int, int> getHSV();
};
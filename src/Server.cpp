#include "Server.h"
#include "Alphabot.h"
#include "Graffiti.h"
#include "ImageProcessing.h"

using namespace std;
using namespace cv;

#define MIN_CONTOUR_AREA 200
#define M_PI 3.1416
#define dAngle 20
#define dDist 10

tuple<int, int> Server::searchForGraffiti()
{
    Mat maskGreen;
    inRange(hsv, lowerHSV[1], upperHSV[1], maskGreen);

    // Накладываем маску на изображение
    graffitiRes = Mat::zeros(frame.size(), CV_8UC3);
    frame.copyTo(graffitiRes, maskGreen);

    int x = 0;
    int y = 0;

    // Поиск контуров на изображении
    vector<vector<Point>> contours;
    findContours(maskGreen, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // Если найден хотя бы один контур
    if (contours.size() > 0)
    {
        // Находим контур с максимальной площадью
        auto max_contour = max_element(contours.begin(), contours.end(),
                                       [](const auto &a, const auto &b)
                                       {
                                           return contourArea(a) < contourArea(b);
                                       });

        // Если площадь контура больше заданного значения
        if (contourArea(*max_contour) > MIN_CONTOUR_AREA)
        {
            // Находим центр контура
            Moments m = moments(*max_contour);
            x = static_cast<int>(m.m10 / m.m00);
            y = static_cast<int>(m.m01 / m.m00);

            // Рисуем белую точку в центре контура
            circle(graffitiRes, Point(x, y), 1, Scalar(255, 255, 255), 2);

            // Возвращаем координаты контура
            return make_tuple(x, y);
        }
    }
    return make_tuple(-1, -1);
}

double Server::AlphabotAngleCalc(int purple_X, int purple_Y, int blue_X, int blue_Y, int target_X, int target_Y)
{
    // Проеряем условия
    if (purple_X == 0 || purple_Y == 0 || blue_Y == 0 || blue_X == 0)
        return 0;
    if (purple_Y - blue_Y == 0)
        return 90;
    if (purple_X == blue_X && blue_Y < purple_Y)
        return 180;

    double center_X = (purple_X + blue_X) / 2.0;
    double center_Y = (purple_Y + blue_Y) / 2.0;

    int dx1 = purple_X - blue_X;
    int dy1 = purple_Y - blue_Y;
    double dx2 = target_X - center_X;
    double dy2 = target_Y - center_Y;

    double det = dx1 * dy2 - dx2 * dy1;
    double dot = dx1 * dx2 + dy1 * dy2;

    double angle_rad = atan2(det, dot);
    double angle_deg = angle_rad * 180 / M_PI;

    // Возвращаем угол в градусах с точностью до двух знаков
    return round(-angle_deg * 100) / 100;
}

void Server::drawGraphics(int purple_X, int purple_Y, int blue_X, int blue_Y, double angle, bool purpleContour, bool blueContour, Graffiti graffiti)
{
    if (purpleContour)
    {
        // Рисуем маленький красный круг в центре масс контура фиолетового маркера
        circle(robotRes, Point(purple_X, purple_Y), 10, Scalar(0, 0, 255), -1);
    }
    if (blueContour)
    {
        // Рисуем маленький синий круг в центре масс контура синего маркера
        circle(robotRes, Point(blue_X, blue_Y), 10, Scalar(255, 0, 0), -1);
    }

    // Рисуем маленький белый круг по центру цели
    if (graffiti.isExist())
        circle(robotRes, Point(graffiti.getPosX(), graffiti.getPosY()), 10, Scalar(255, 255, 255), -1);

    if (purpleContour && blueContour && graffiti.isExist())
    {
        // Рисуем линию, соединяющую центр робота и фиолетовый маркер
        line(robotRes, Point(alphabot.getPosX(), alphabot.getPosY()), Point(purple_X, purple_Y), Scalar(255, 255, 255), 1);

        // Рисуем линию, соединяющую центр робота и центр граффити
        line(robotRes, Point(alphabot.getPosX(), alphabot.getPosY()), Point(graffiti.getPosX(), graffiti.getPosY()), Scalar(255, 255, 255), 1);
    
        // Пишем текст с углом поворота
        putText(robotRes, to_string(angle), Point(alphabot.getPosX(), alphabot.getPosY()), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    }
}

void Server::searchForRobot(Graffiti graffiti)
{
    // Фильтруем кадр по цвету
    Mat maskPurple, maskBlue;

    inRange(hsv, lowerHSV[0], upperHSV[0], maskPurple);
    inRange(hsv, lowerHSV[2], upperHSV[2], maskBlue);

    Mat mask = maskBlue | maskPurple;

    // Накладываем маску на исходный кадр и записываем результат в result
    robotRes = Mat::zeros(frame.size(), CV_8UC3);
    frame.copyTo(robotRes, mask);

    int purple_X = 0;
    int purple_Y = 0;
    int blue_X = 0;
    int blue_Y = 0;

    // Поиск контуров объектов на маске с фильтрованным фиолетовым цветом
    vector<vector<Point>> contoursPurple;
    findContours(maskPurple, contoursPurple, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // Если найден хотя бы один контур
    if (contoursPurple.size() > 0)
    {
        // Находим контур с максимальной площадью
        auto max_contour = max_element(contoursPurple.begin(), contoursPurple.end(),
                                       [](const auto &a, const auto &b)
                                       {
                                           return contourArea(a) < contourArea(b);
                                       });
        // Если контур больше минимального значения
        if (contourArea(*max_contour) > MIN_CONTOUR_AREA)
        {
            // Находим координаты центра масс контура
            Moments M = moments(*max_contour);
            if (M.m00 != 0)
            {
                purple_X = static_cast<int>(M.m10 / M.m00);
                purple_Y = static_cast<int>(M.m01 / M.m00);
            }
        }
    }

    // Поиск контуров объектов на маске с фильтрованным синим цветом
    vector<vector<Point>> contoursBlue;
    findContours(maskBlue, contoursBlue, RETR_TREE, CHAIN_APPROX_SIMPLE);
    // Если найден хотя бы один контур
    if (contoursBlue.size() > 0)
    {
        // Находим контур с максимальной площадью
        auto max_contour = max_element(contoursBlue.begin(), contoursBlue.end(),
                                       [](const auto &a, const auto &b)
                                       {
                                           return contourArea(a) < contourArea(b);
                                       });
        // Если контур больше минимального значения
        if (contourArea(*max_contour) > MIN_CONTOUR_AREA)
        {
            // Находим координаты центра масс контура
            Moments M = moments(*max_contour);
            if (M.m00 != 0)
            {
                blue_X = static_cast<int>(M.m10 / M.m00);
                blue_Y = static_cast<int>(M.m01 / M.m00);
            }
        }
    }

    if (purple_X == 0 || purple_Y == 0 || blue_X == 0 || blue_Y == 0)
    {
        alphabot.SetPosition(0, 0);
        return;
    }

    // Если найдены оба контура
    if (contoursPurple.size() > 0 && contoursBlue.size() > 0)
    {
        // Вычисляем угол между линиями и выводим его
        angle = AlphabotAngleCalc(purple_X, purple_Y, blue_X, blue_Y, graffiti.getPosX(), graffiti.getPosY());
    }

    // Рисуем графику
    drawGraphics(purple_X, purple_Y, blue_X, blue_Y, angle, contoursPurple.size() > 0, contoursBlue.size() > 0, graffiti);

    alphabot.SetPosition((purple_X + blue_X) / 2, (purple_Y + blue_Y) / 2);
    return;
}

void Server::choseAction()
{
    // Если граффити не найдено или двигатель запущен, то робот ничего не делает
    if (!graffiti.isExist() || !alphabot.isEngineStarted())
    {
        alphabot.setState(4); // IDLE
        return;
    }

    // Если угол больше порогового значения, то поворачиваем робота
    if (abs(angle) > dAngle)
    {
        if (angle > 0)
        {
            alphabot.setState(2); // RIGHT
            return;
        }
        else
        {
            alphabot.setState(1); // LEFT
            return;
        }
    }

    // Вычисляем расстояние до граффити
    double dist = sqrt(pow(graffiti.getPosX() - alphabot.getPosX(), 2) + pow(graffiti.getPosY() - alphabot.getPosY(), 2));

    // Если расстояние больше порогового значения, то двигаемся к граффити
    if (dist > dDist)
    {
        alphabot.setState(0); // FORWARD
        return;
    }
    // Иначе мы находимся рядом с граффити, и робот начинает его чистить
    else
    {
        alphabot.setState(3); // CLEAN
        return;
    }
}

void Server::showResults()
{
    // Отображаем результаты
    imshow("Original", frame);
    imshow("Robot", robotRes);
    imshow("Graffiti", graffitiRes);
}

Server::Server() {}

void Server::receiveMessage()
{
    // Получаем изображение с камеры
    tie(hsv, frame) = camera.sendToServer();

    if (debugMode)
        return;

    // Ищем граффити
    int xGraffiti, yGraffiti;
    tie(xGraffiti, yGraffiti) = searchForGraffiti();

    // Создаем объект граффити
    if (xGraffiti != -1 && yGraffiti != -1)
    {
        graffiti.setState(xGraffiti, yGraffiti, true);
    }
    else
    {
        graffiti.setState(-1, -1, false);
    }

    // Вычисляем позицию робота
    searchForRobot(graffiti);

    // Выбираем действие
    choseAction();

    if (!alphabot.isEngineStarted() && alphabot.getPosX() != 0 && alphabot.getPosY() != 0)
        alphabot.startEngine();
    else if (alphabot.isEngineStarted() && (alphabot.getPosX() == 0 || alphabot.getPosY() == 0))
        alphabot.stopEngine();

    // Отображаем результаты
    showResults();
}

void Server::sendImageToThresholdGenerator()
{
    // Отправляем изображение на генератор порогов
    thresholdGenerator.recieveImage(hsv, frame);
}

void Server::sendHSVToThresholdGenerator()
{
    thresholdGenerator.saveHSVtoFile(lowerHSV, upperHSV);
}

void Server::recieveHSVFromThresholdGenerator()
{
    cout << "Loaded HSV from file" << endl;
    vector<vector<Scalar>> tmp = thresholdGenerator.sendHSVtoServer();
    lowerHSV = tmp[0];
    upperHSV = tmp[1];
    cout << "Low " << lowerHSV[0] << " " << lowerHSV[1] << " " << lowerHSV[2] << endl;
    cout << "Up  " << upperHSV[0] << " " << upperHSV[1] << " " << upperHSV[2] << endl;
}

void Server::setDebugMode(bool isDebug)
{
    debugMode = isDebug;

    // Если включен режим отладки, должны быть только окна Threshold и Mask
    if (debugMode)
    {
        thresholdGenerator.trackBar();
        namedWindow("Mask", WINDOW_NORMAL);

        destroyWindow("Original");
        destroyWindow("Robot");
        destroyWindow("Graffiti");
    }
    else
    {
        namedWindow("Original", WINDOW_NORMAL);
        namedWindow("Robot", WINDOW_NORMAL);
        namedWindow("Graffiti", WINDOW_NORMAL);

        destroyWindow("Threshold");
        destroyWindow("Mask");
        destroyWindow("HUE");
    }
}

void Server::setHSV(string color)
{
    int hMin, sMin, vMin, hMax, sMax, vMax;
    tie(hMin, sMin, vMin, hMax, sMax, vMax) = thresholdGenerator.getHSV();
    cout << "New HSV for " << color << ": " << hMin << " " << sMin << " " << vMin << " " << hMax << " " << sMax << " " << vMax << endl;
    if (color == "purple")
    {
        lowerHSV[0] = Scalar(hMin, sMin, vMin);
        upperHSV[0] = Scalar(hMax, sMax, vMax);
    }
    else if (color == "green")
    {
        lowerHSV[1] = Scalar(hMin, sMin, vMin);
        upperHSV[1] = Scalar(hMax, sMax, vMax);
    }
    else if (color == "blue")
    {
        lowerHSV[2] = Scalar(hMin, sMin, vMin);
        upperHSV[2] = Scalar(hMax, sMax, vMax);
    }
}

void Server::alphabotRun()
{
    alphabot.run();
}
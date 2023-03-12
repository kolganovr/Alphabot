#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <tuple>
#include <vector>

using namespace cv;
using namespace std;

#define MIN_CONTOUR_AREA 200
#define M_PI 3.1416
#define dAngle 20
#define dDist 10

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
    void moveForward(int time)
    {
        cout << "moveForward" << endl;
    }
    // Выполняет поворот налево заданное время
    void moveLeft(int time)
    {
        cout << "moveLeft" << endl;
    }
    // Выполняет поворот направо заданное время
    void moveRight(int time)
    {
        cout << "moveRight" << endl;
    }
};

// Класс для управления моющими щетками
class Cleaner
{
public:
    // Выполняет очистку поверхности
    void perform()
    {
        cout << "Clean" << endl;
    }
};

// Класс для управления камерой
class Camera
{
private:
    // VideoCapture объект для считывания изображения с камеры
    VideoCapture cap;

    /// Получает изображение с камеры
    /// @return Кортеж из двух матриц -- HSV и frame
    tuple<Mat, Mat> getImage()
    {
        // Считываем кадр
        Mat frame;
        cap.read(frame);

        // Переводим кадр в HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Возвращаем HSV и frame
        return make_tuple(hsv, frame);
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
        namedWindow("Robot", WINDOW_NORMAL);
        namedWindow("Graffiti", WINDOW_NORMAL);
    }

    ~Camera()
    {
        // Закрываем камеру
        cap.release();
        destroyAllWindows();
    }

    /// Отправляем изображение на сервер
    /// @return Кортеж из двух матриц -- HSV и frame
    tuple<Mat, Mat> sendToServer()
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

    /// Устанавливает позицию граффити
    /// @param x X позиция
    /// @param y Y позиция
    void setState(int x, int y, bool exists)
    {
        if (x < 0 || y < 0)
        {
            exist = false;
            return;
        }
        posX = x;
        posY = y;
        exist = exists;
    }
    /// Возвращает X позицию граффити, если существует, иначе -1
    /// @return Позиция X -- 0 Левая сторона
    int getPosX() { return exist ? posX : -1; }
    /// Возвращает Y позицию граффити (если существует, иначе -1)
    /// @return Позиция Y -- 0 верхняя сторона
    int getPosY() { return exist ? posY : -1; }
    /// Возвращает существование граффити
    /// @return true если граффити существует
    bool isExist() { return exist; }
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
    void doAction(int time)
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

public:
    Alphabot()
    {
        engineStarted = false;
        state = IDLE;
        posX = 0;
        posY = 0;
    }

    /// @brief Устанавливает позицию робота
    /// @param x X позиция
    /// @param y Y позиция
    void SetPosition(int x, int y)
    {
        posX = x;
        posY = y;
    }

    /// Получает X позицию робота
    /// @return X позиция
    int getPosX() { return posX; }
    /// Получает Y позицию робота
    /// @return Y позиция
    int getPosY() { return posY; }

    // Запускает двигатель
    void startEngine()
    {
        cout << "Engine started" << endl;
        engineStarted = true;
    }

    // Останавливает двигатель
    void stopEngine()
    {
        cout << "Engine stopped" << endl;
        engineStarted = false;
    }

    /// Возвращает состояние двигателя
    /// @return true если двигатель запущен
    bool isEngineStarted()
    {
        return engineStarted;
    }

    /// Устанавливает состояние робота
    /// @param state Состояние робота
    void setState(Action state)
    {
        this->state = state;
    }

    // Опрашивает робота и вызывает выполнение действия
    void run()
    {
        // Если двигатель не запущен или робот находится в состоянии IDLE, то ничего не делаем
        if (!engineStarted || state == 4)
        {
            return;
        }
        doAction(1);
    }
};

int hMin = 0, hMax = 179, sMin = 0, sMax = 255, vMin = 0, vMax = 255;
class ThresholdGenerator
{
private:
    Mat mask;
    Mat hsv;
    Mat frame;

public:
    ThresholdGenerator(){};

    /// Функция для создания трекбаров для настройки фильтра HSV
    void trackBar()
    {
        namedWindow("Threshold", WINDOW_NORMAL);
        // Создаем трекбары для настройки фильтра
        createTrackbar("Hue Min", "Threshold", &hMin, 179);
        createTrackbar("Hue Max", "Threshold", &hMax, 179);
        createTrackbar("Sat Min", "Threshold", &sMin, 255);
        createTrackbar("Sat Max", "Threshold", &sMax, 255);
        createTrackbar("Val Min", "Threshold", &vMin, 255);
        createTrackbar("Val Max", "Threshold", &vMax, 255);
    }

    /// Функция для сохранений значений маски в файл
    /// @param hsvValues вектор значений маски для каждого цвета (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    void saveHSVtoFile(vector<Scalar> hsvValues)
    {
        ofstream file;
        file.open("../../docs/hsvValues.txt");

        if (!file.is_open())
        {
            cout << "Error opening file" << endl;
            return;
        }

        // Записываем в файл значения нижней и верхней границы цветов
        for (int i = 0; i < hsvValues.size(); i++)
        {
            file << hsvValues[i][0] << " " << hsvValues[i][1] << " " << hsvValues[i][2] << endl;
        }
        cout << "HSV values saved to file" << endl;
        file.close();
    }

    /// Функция для получения значений маски из файла
    /// @return вектор значений маски для каждого цвета (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    tuple<Scalar, Scalar, Scalar, Scalar, Scalar, Scalar> getHSVfromFile()
    {
        ifstream file;
        file.open("../../docs/hsvValues.txt");
        if (!file.is_open())
        {
            cout << "ERROR OPPENING FILE!" << endl;
        }

        Scalar lower_purple, upper_purple, lower_green, upper_green, lower_blue, upper_blue;
        file >> lower_purple[0] >> lower_purple[1] >> lower_purple[2];
        file >> upper_purple[0] >> upper_purple[1] >> upper_purple[2];
        file >> lower_green[0] >> lower_green[1] >> lower_green[2];
        file >> upper_green[0] >> upper_green[1] >> upper_green[2];
        file >> lower_blue[0] >> lower_blue[1] >> lower_blue[2];
        file >> upper_blue[0] >> upper_blue[1] >> upper_blue[2];
        file.close();
        cout << "HSV values loaded from file" << endl;

        cout << "lower_purple: " << lower_purple << " "
             << "upper_purple: " << upper_purple << endl;
        cout << "lower_green:  " << lower_green << " "
             << "upper_green:  " << upper_green << endl;
        cout << "lower_blue:   " << lower_blue << " "
             << "upper_blue:   " << upper_blue << endl;

        return make_tuple(lower_purple, upper_purple, lower_green, upper_green, lower_blue, upper_blue);
    }

    /// Функция для получения изображения с камеры
    void recieveImage(const Mat &hsv, const Mat &frame)
    {
        this->hsv = hsv;
        this->frame = frame;

        // Задаем маску
        inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), mask);

        // Отображаем результаты
        imshow("Mask", mask);
    }

    /// Функция для отправки маски на сервер
    /// @return параметры маски (hMin, sMin, vMin, hMax, sMax, vMax) в порядке: красный, зеленый, синий
    tuple<Scalar, Scalar, Scalar, Scalar, Scalar, Scalar> sendHSVtoServer()
    {
        return getHSVfromFile();
    }
};

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

    // Уставнавливаем нижнюю и верхнюю границу цветов маркера
    Scalar lower_purple = Scalar(150, 146, 246);
    Scalar upper_purple = Scalar(179, 219, 255);

    Scalar lower_green = Scalar(50, 116, 40);
    Scalar upper_green = Scalar(93, 222, 147);

    Scalar lower_blue = Scalar(91, 179, 125);
    Scalar upper_blue = Scalar(108, 255, 255);

    /// Поиск граффити на изображении
    /// @return координаты граффити
    tuple<int, int> searchForGraffiti()
    {
        Mat maskGreen;
        inRange(hsv, lower_green, upper_green, maskGreen);

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

    /// Вычисляет угол наклона робота
    /// @param puprle_X X координата фиолетовой точки
    /// @param purple_Y Y координата фиолетовой точки
    /// @param blue_X X координата синей точки
    /// @param blue_Y Y координата синей точки
    /// @return угол наклона в градусах
    double AlphabotAngleCalc(int purple_X, int purple_Y, int blue_X, int blue_Y, int target_X, int target_Y)
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

    /// Ристует графику поверх изображения
    /// @param blue_X X позиция синего маркера
    /// @param blue_Y Y позиция синего маркера
    /// @param purple_X X позиция фиолетового маркера
    /// @param purple_Y Y позиция фиолетового маркера
    /// @param angle угол
    /// @param purpleContour фиолетовый маркер найден
    /// @param blueContour синий маркер найден
    void drawGraphics(int purple_X, int purple_Y, int blue_X, int blue_Y, double angle, bool purpleContour, bool blueContour, Graffiti graffiti)
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

        if (purpleContour && blueContour)
        {
            // Пишем текст с углом поворота
            putText(robotRes, to_string(angle), Point(alphabot.getPosX(), alphabot.getPosY()), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        }

        // Рисуем маленький белый круг по центру цели
        if (graffiti.isExist())
            circle(robotRes, Point(graffiti.getPosX(), graffiti.getPosY()), 10, Scalar(255, 255, 255), -1);

        if (purpleContour && blueContour)
        {

            // Рисуем линию, соединяющую центр робота и фиолетовый маркер
            line(robotRes, Point(alphabot.getPosX(), alphabot.getPosY()), Point(purple_X, purple_Y), Scalar(255, 255, 255), 1);

            // Рисуем линию, соединяющую центр робота и центр граффити
            line(robotRes, Point(alphabot.getPosX(), alphabot.getPosY()), Point(graffiti.getPosX(), graffiti.getPosY()), Scalar(255, 255, 255), 1);
        }
    }

    /// Ищет позцию робота на кадре и устанавливает его позицию
    /// @param graffiti граффити
    void searchForRobot(Graffiti graffiti)
    {
        // Фильтруем кадр по цвету
        Mat maskPurple, maskBlue;
        inRange(hsv, lower_purple, upper_purple, maskPurple);
        inRange(hsv, lower_blue, upper_blue, maskBlue);

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

    // Выбирает действие для робота
    void chooseAction()
    {
        // Если граффити не найдено или двигатель запущен, то робот ничего не делает
        if (!graffiti.isExist() || !alphabot.isEngineStarted())
        {
            alphabot.setState(IDLE);
            return;
        }

        // Если угол больше порогового значения, то поворачиваем робота
        if (abs(angle) > dAngle)
        {
            if (angle > 0)
            {
                alphabot.setState(RIGHT);
                return;
            }
            else
            {
                alphabot.setState(LEFT);
                return;
            }
        }

        // Вычисляем расстояние до граффити
        double dist = sqrt(pow(graffiti.getPosX() - alphabot.getPosX(), 2) + pow(graffiti.getPosY() - alphabot.getPosY(), 2));

        // Если расстояние больше порогового значения, то двигаемся к граффити
        if (dist > dDist)
        {
            alphabot.setState(FORWARD);
            return;
        }
        // Иначе мы находимся рядом с граффити, и робот начинает его чистить
        else
        {
            alphabot.setState(CLEAN);
            return;
        }
    }
    /// @brief Отображает исходный кадр и кадр с результатами фильтрации
    void showResults()
    {
        // Отображаем результаты
        imshow("Original", frame);
        imshow("Robot", robotRes);
        imshow("Graffiti", graffitiRes);
    }

public:
    Server() {}

    // Получает сообщение от камеры, отправляет роботу и отображает результаты
    void receiveMessage()
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
        chooseAction();

        if (!alphabot.isEngineStarted() && alphabot.getPosX() != 0 && alphabot.getPosY() != 0)
            alphabot.startEngine();
        else if (alphabot.isEngineStarted() && (alphabot.getPosX() == 0 || alphabot.getPosY() == 0))
            alphabot.stopEngine();

        // Отображаем результаты
        showResults();
    }

    // Отправляет изображение с камеры в генератор маски
    void sendImageToThresholdGenerator()
    {
        thresholdGenerator.recieveImage(hsv, frame);
    }

    // Отправляет границы цветов в генератор маски для сохранения в файл
    void sendHSVToThresholdGenerator()
    {
        thresholdGenerator.saveHSVtoFile(vector<Scalar>{lower_purple, upper_purple, lower_green, upper_green, lower_blue, upper_blue});
    }

    // Получает границы цветов из генератора маски
    void recieveHSVFromThresholdGenerator()
    {
        tie(lower_purple, upper_purple, lower_green, upper_green, lower_blue, upper_blue) = thresholdGenerator.sendHSVtoServer();
    }

    /// Устанавливает режим отладки
    /// @param isDebug - true, если включен режим отладки
    void setDebugMode(bool isDebug)
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
        }
    }

    /// Устанавливает границы цветов
    /// @param color - цвет, границы которого нужно установить (purple, blue, green)
    void setHSV(string color)
    {
        cout << color << " " << hMin << " " << sMin << " " << vMin << " " << hMax << " " << sMax << " " << vMax << endl;
        if (color == "purple")
        {
            lower_purple = Scalar(hMin, sMin, vMin);
            upper_purple = Scalar(hMax, sMax, vMax);
        }
        if (color == "blue")
        {
            lower_blue = Scalar(hMin, sMin, vMin);
            upper_blue = Scalar(hMax, sMax, vMax);
        }
        if (color == "green")
        {
            lower_green = Scalar(hMin, sMin, vMin);
            upper_green = Scalar(hMax, sMax, vMax);
        }
    }

    // Запускает опрос робота
    void alphabotRun()
    {
        alphabot.run();
    }
};

int main()
{
    bool isDebug = false;
    Server server;

    while (true)
    {
        // Получаем нажатую клавишу
        char key = waitKey(1);

        // Если нажат пробел, то переключаем режим отладки
        if (key == 32)
        {
            isDebug = !isDebug;
            server.setDebugMode(isDebug);
            cout << "Debug mode: " << isDebug << endl;
        }

        // Получаем сообщение от камеры
        server.receiveMessage();

        if (isDebug)
        {
            server.sendImageToThresholdGenerator();

            // Если нажата клавиша, то сохраняем настройки пороговых значений для заданного цвета
            if (key == 49) // 1
            {
                server.setHSV("purple");
            }
            else if (key == 50) // 2
            {
                server.setHSV("green");
            }
            else if (key == 51) // 3
            {
                server.setHSV("blue");
            }
            else if (key == 115) // s
            {
                server.sendHSVToThresholdGenerator();
            }
            else if (key == 108) // l
            {
                server.recieveHSVFromThresholdGenerator();
            }
        }
        else
        {
            // Опрашиваем робота
            server.alphabotRun();
        }

        // Если нажата ESC, то выходим из программы
        if (key == 27)
        {
            break;
        }
    }
    return 0;
}
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <tuple>

using namespace cv;
using namespace std;

#define MIN_CONTOUR_AREA 200
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
    bool engineStarted;
    EngineController engineController;
    Cleaner cleaner;
    int posX;        // Текущая X координата
    int posY;        // Текущая Y координата
    float angle = 0; // Текущий угол поворота
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
    Alphabot()
    {
        engineStarted = false;
        posX = 0;
        posY = 0;
    }

    /// Получает сообщенеи с сервера
    /// @param target граффити к кторому нужно двигаться
    void recieveMessage(Graffiti graffiti, int x, int y, float agl)
    {
        target = graffiti;
        posX = x;
        posY = y;
        angle = agl;
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

    void startEngine()
    {
        engineStarted = true;
    }
    void stopEngine()
    {
        engineStarted = false;
    }
};

int hMin = 0, hMax = 179, sMin = 0, sMax = 255, vMin = 0, vMax = 255;
class TresholdGenerator
{
private:
    Mat mask;
    Mat hsv;
    Mat frame;

public:
    TresholdGenerator()
    {
        namedWindow("Mask", WINDOW_NORMAL);
        namedWindow("Treshhold", WINDOW_NORMAL);
    }

    void trackBar()
    {
        // Создаем трекбары для настройки фильтра
        createTrackbar("Hue Min", "Treshhold", &hMin, 179);
        createTrackbar("Hue Max", "Treshhold", &hMax, 179);
        createTrackbar("Sat Min", "Treshhold", &sMin, 255);
        createTrackbar("Sat Max", "Treshhold", &sMax, 255);
        createTrackbar("Val Min", "Treshhold", &vMin, 255);
        createTrackbar("Val Max", "Treshhold", &vMax, 255);
    }

    void showMask()
    {
        // Задаем маску
        inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), mask);

        // Отображаем результаты
        imshow("Mask", hsv);
        // imshow("Trackbars", Mat::zeros(Size(300, 200), CV_8UC1));
    }

    void printHSV()
    {
        cout << "Hue Min: " << hMin << endl;
        cout << "Hue Max: " << hMax << endl;
        cout << "Sat Min: " << sMin << endl;
        cout << "Sat Max: " << sMax << endl;
        cout << "Val Min: " << vMin << endl;
        cout << "Val Max: " << vMax << endl;
    }

    void recieveImage(const Mat &hsv, const Mat &frame)
    {
        this->hsv = hsv;
        this->frame = frame;

        // Задаем маску
        inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), mask);
        cout << "hMin: " << hMin << endl;

        // Отображаем результаты
        imshow("Mask", mask);
    }
};

// Класс сервера для коммуникации между камерой и роботом а также вычислений
class Server
{
private:
    Alphabot alphabot;
    Graffiti graffiti;
    Camera camera;
    TresholdGenerator tresholdGenerator;
    Mat frame;
    Mat hsv;
    Mat robotRes;
    Mat graffitiRes;

    // Уставнавливаем нижнюю и верхнюю границу цветов маркера
    Scalar lower_purple = convert_hsv(Scalar(300, 30, 30));
    Scalar upper_purple = convert_hsv(Scalar(350, 100, 100));

    Scalar lower_blue = convert_hsv(Scalar(180, 50, 30));
    Scalar upper_blue = convert_hsv(Scalar(220, 100, 100));

    Scalar lower_green = convert_hsv(Scalar(150, 30, 30));
    Scalar upper_green = convert_hsv(Scalar(180, 80, 80));

    /// Переводит HSV в формат [0, 180] [0, 255] [0, 255]
    /// @param hsv HSV в формате [0, 360] [0, 100] [0, 100]
    /// @return HSV в формате [0, 180] [0, 255] [0, 255]
    Scalar convert_hsv(Scalar hsv)
    {
        // Получаем H S V парметры
        int h = hsv[0];
        int s = hsv[1];
        int v = hsv[2];

        // Переводим H из формата [0, 360] в [2, 180]
        h = static_cast<int>(h * 180 / 360);

        // Переводим S и V из формата [0, 100] в [0, 255]
        s = static_cast<int>(s * 255 / 100);
        v = static_cast<int>(v * 255 / 100);

        // Возвращаем HSV
        return Scalar(h, s, v);
    }

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

    /// Ищет позицию робота
    /// @return Кортеж из двух чисел -- X и Y позиция робота
    tuple<int, int, float> searchForRobot(Graffiti graffiti)
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

        double angle = 0; // Угол поворота робота (по умолчанию 0)
        // Если найдены оба контура
        if (contoursPurple.size() > 0 && contoursBlue.size() > 0)
        {
            // Вычисляем угол между линиями и выводим его
            angle = AlphabotAngleCalc(purple_X, purple_Y, blue_X, blue_Y, graffiti.getPosX(), graffiti.getPosY());
        }

        // Рисуем графику
        drawGraphics(purple_X, purple_Y, blue_X, blue_Y, angle, contoursPurple.size() > 0, contoursBlue.size() > 0, graffiti);

        return {static_cast<float>((purple_X + blue_X) / 2), static_cast<float>((purple_Y + blue_Y) / 2), angle};
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

        // Ищем граффити
        int xGraffiti, yGraffiti;
        tie(xGraffiti, yGraffiti) = searchForGraffiti();

        // Создаем объект граффити
        graffiti.setState(xGraffiti, yGraffiti, true);

        // Вычисляем позицию робота
        int x, y;
        float angle;
        tie(x, y, angle) = searchForRobot(graffiti);

        // Отображаем результаты
        showResults();

        // Отправляем сообщение роботу
        sendMessage(graffiti, x, y, angle);
    }

    // Отправляет сообщение роботу
    void sendMessage(Graffiti graffiti, int x, int y, float angle = 0)
    {
        alphabot.recieveMessage(graffiti, x, y, angle);
    }

    TresholdGenerator getTresholdGenerator()
    {
        return tresholdGenerator;
    }
    void sendImageToTresholdGenerator()
    {
        tresholdGenerator.recieveImage(hsv, frame);
    }
};

int main()
{
    Server server;
    TresholdGenerator tresholdGenerator = server.getTresholdGenerator();
    tresholdGenerator.trackBar();

    while (true)
    {
        // Получаем сообщение от камеры
        server.receiveMessage();

        if (true)
        {
            server.sendImageToTresholdGenerator();
            // tresholdGenerator.showMask();
        }

        // Ожидаем нажатия клавиши
        // При нажатии на esc выходим
        if (waitKey(1) == 27)
        {
            // tresholdGenerator.printHSV();
            break;
        }
    }
    return 0;
}
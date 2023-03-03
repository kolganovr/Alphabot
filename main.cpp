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
    tuple<Mat, Mat> getImage()
    {
        // Считываем кадр
        Mat frame;
        cap.read(frame);

        // Переводим кадр в HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Отображаем исходный кадр
        // imshow("Original", frame);

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
    Alphabot()
    {
        engineStarted = false;
        posX = 0;
        posY = 0;
    }
    /// Получает сообщенеи с сервера
    /// @param target граффити к кторому нужно двигаться
    void recieveMessage(Graffiti graffiti, int x, int y)
    {
        target = graffiti;
        posX = x;
        posY = y;
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
    Mat frame;
    Mat hsv;
    Mat result;

    // Уставнавливаем нижнюю и верхнюю границу цветов маркера
    Scalar lower_purple = convert_hsv(Scalar(300, 30, 30));
    Scalar upper_purple = convert_hsv(Scalar(350, 100, 100));

    Scalar lower_blue = convert_hsv(Scalar(180, 50, 30));
    Scalar upper_blue = convert_hsv(Scalar(220, 100, 100));

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

    /// Ищет граффити
    /// @return Graffiti объект граффити
    Graffiti searchForGraffiti();
    
    /// Вычисляет угол наклона робота
    /// @param puprle_X X координата фиолетовой точки
    /// @param purple_Y Y координата фиолетовой точки
    /// @param blue_X X координата синей точки
    /// @param blue_Y Y координата синей точки
    /// @return угол наклона в градусах
    double AlphabotAngleCalc(int purple_X, int purple_Y, int blue_X, int blue_Y)
    {
        // Проеряем условия
        if (purple_X == 0 || purple_Y == 0 || blue_Y == 0 || blue_X == 0)
            return 0;
        if (purple_Y - blue_Y == 0)
            return 90;
        if (purple_X == blue_X && blue_Y < purple_Y)
            return 180;

        // Вычисляем угол наклона
        double slope = static_cast<double>(purple_X - blue_X) / static_cast<double>(blue_Y - purple_Y);

        double angle_rad = atan(slope);
        double angle_deg = angle_rad * 180 / M_PI;

        if (angle_deg < 0 && blue_Y < purple_Y)
            angle_deg = 180 + angle_deg;
        else if (blue_X > purple_X && blue_Y < purple_Y)
            angle_deg = -180 + angle_deg;

        // Возвращаем угол в градусах с точностью до двух знаков
        return round(angle_deg * 100) / 100;
    }

    /// Ристует графику поверх изображения
    /// @param blue_X X позиция синего маркера
    /// @param blue_Y Y позиция синего маркера
    /// @param purple_X X позиция фиолетового маркера
    /// @param purple_Y Y позиция фиолетового маркера
    /// @param angle угол
    /// @param purpleContour фиолетовый маркер найден
    /// @param blueContour синий маркер найден
    void drawGraphics(int purple_X, int purple_Y, int blue_X, int blue_Y, double angle, bool purpleContour, bool blueContour)
    {
        if (purpleContour)
        {
            // Рисуем маленький красный круг в центре масс контура фиолетового маркера
            circle(result, Point(purple_X, purple_Y), 10, Scalar(0, 0, 255), -1);
        }
        if (blueContour)
        {
            // Рисуем маленький синий круг в центре масс контура синего маркера
            circle(result, Point(blue_X, blue_Y), 10, Scalar(255, 0, 0), -1);
        }

        if (purpleContour && blueContour)
        {
            // Рисуем вертикальную линию из цетра масс синего цвета
            line(result, Point(blue_X, blue_Y), Point(blue_X, 0), Scalar(256, 256, 256), 1);

            // Рисуем линию, соединяющую центры масс
            line(result, Point(purple_X, purple_Y), Point(blue_X, blue_Y), Scalar(256, 256, 256), 1);

            // Пишем текст с углом поворота
            putText(result, to_string(angle), Point(blue_X, blue_Y), FONT_HERSHEY_SIMPLEX, 1, Scalar(256, 256, 256), 2);
        }
    }

    /// Ищет позицию робота
    /// @return Кортеж из двух чисел -- X и Y позиция робота
    tuple<int, int> searchForRobot()
    {
        // Фильтруем кадр по цвету
        Mat maskPurple, maskBlue;
        inRange(hsv, lower_purple, upper_purple, maskPurple);
        inRange(hsv, lower_blue, upper_blue, maskBlue);

        Mat mask = maskBlue | maskPurple;

        // Накладываем маску на исходный кадр и записываем результат в result
        result = Mat::zeros(frame.size(), CV_8UC3);
        frame.copyTo(result, mask);

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
            angle = AlphabotAngleCalc(purple_X, purple_Y, blue_X, blue_Y);
        }

        // Рисуем графику
        drawGraphics(purple_X, purple_Y, blue_X, blue_Y, angle, contoursPurple.size() > 0, contoursBlue.size() > 0);
        
        return {static_cast<float>((purple_X + blue_X)/2), static_cast<float>((purple_Y + blue_Y)/2)};
    }

    void showResults()
    {
        // Отображаем результаты
        imshow("Original", frame);
        imshow("Filtered", result);
    }

public:
    Server() {}

    // Получает сообщение от камеры, отправляет роботу и отображает результаты
    void receiveMessage()
    {
        // Получаем изображение с камеры
        tie(hsv, frame) = camera.sendToServer();

        // Ищем граффити
        // Graffiti graffiti = searchForGraffiti();

        // Вычисляем позицию робота
        searchForRobot();

        // Отображаем результаты
        showResults();

        // Отправляем сообщение роботу
        // sendMessage(graffiti);
    }
    // Отправляет сообщение роботу
    void sendMessage(Graffiti graffiti, int x, int y)
    {
        alphabot.recieveMessage(graffiti, x, y);
    }
};

int main()
{
    Server server;
    while (true)
    {
        // Получаем сообщение от камеры
        server.receiveMessage();

        // Ожидаем нажатия клавиши
        // При нажатии на esc выходим
        if (waitKey(1) == 27)
            break;
    }
    return 0;
}
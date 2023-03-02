#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

#define MIN_CONTOUR_AREA 250

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

double angle_between_centers(int cx_purple, int cy_purple, int cx_blue, int cy_blue)
{
    // Проеряем условия
    if (cx_purple == 0 || cy_purple == 0 || cy_blue == 0 || cx_blue == 0)
        return 0;
    if (cy_purple - cy_blue == 0)
        return 90;
    if (cx_purple == cx_blue && cy_blue < cy_purple)
        return 180;

    // Вычисляем угол наклона
    double slope = static_cast<double>(cx_purple - cx_blue) / static_cast<double>(cy_blue - cy_purple);

    double angle_rad = atan(slope);
    double angle_deg = angle_rad * 180 / M_PI;

    if (angle_deg < 0 && cy_blue < cy_purple)
        angle_deg = 180 + angle_deg;
    else if (cx_blue > cx_purple && cy_blue < cy_purple)
        angle_deg = -180 + angle_deg;

    // Возвращаем угол в градусах с точностью до двух знаков
    return round(angle_deg * 100) / 100;
}

int main()
{
    // Создаем VideoCapture объект для считывания данных с камеры
    VideoCapture cap(0);

    // Проверяем, удалось ли открыть камеру
    if (!cap.isOpened())
    {
        cout << "Error: Could not initialize the video capture." << endl;
        return -1;
    }

    // Создаем окна для отображения исходного кадра и фильтрованного
    namedWindow("Original", WINDOW_NORMAL);
    namedWindow("Filtered", WINDOW_NORMAL);

    while (true)
    {
        // Считываем кадр
        Mat frame;
        cap.read(frame);

        // Переводим кадр в HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // Уставнавливаем нижнюю и верхнюю границу цветов маркера
        Scalar lower_purple = convert_hsv(Scalar(300, 30, 30));
        Scalar upper_purple = convert_hsv(Scalar(350, 100, 100));

        Scalar lower_blue = convert_hsv(Scalar(180, 50, 30));
        Scalar upper_blue = convert_hsv(Scalar(220, 100, 100));

        // Фильтруем кадр по цвету
        Mat maskPurple, maskBlue;
        inRange(hsv, lower_purple, upper_purple, maskPurple);
        inRange(hsv, lower_blue, upper_blue, maskBlue);

        Mat mask = maskBlue | maskPurple;

        // Накладываем маску на исходный кадр
        Mat result;
        bitwise_and(frame, frame, result, mask);

        int cx_purple = 0;
        int cy_purple = 0;
        int cx_blue = 0;
        int cy_blue = 0;

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
                    cx_purple = static_cast<int>(M.m10 / M.m00);
                    cy_purple = static_cast<int>(M.m01 / M.m00);
                    // Рисуем маленький красный круг в центре масс контура
                    circle(result, Point(cx_purple, cy_purple), 10, Scalar(0, 0, 255), -1);
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
                    cx_blue = static_cast<int>(M.m10 / M.m00);
                    cy_blue = static_cast<int>(M.m01 / M.m00);
                    // Рисуем маленький синий круг в центрe
                    circle(result, Point(cx_blue, cy_blue), 10, Scalar(255, 0, 0), -1);
                }
            }
        }

        // Если найдены оба контура
        if (contoursPurple.size() > 0 && contoursBlue.size() > 0)
        {
            // РИсуем вертикальную линию из цетра масс синего цвета
            line(result, Point(cx_blue, cy_blue), Point(cx_blue, 0), Scalar(256, 256, 256), 1);

            // Рисуем линию, соединяющую центры масс
            line(result, Point(cx_purple, cy_purple), Point(cx_blue, cy_blue), Scalar(256, 256, 256), 1);

            // Вычисляем угол между линиями и выводим его
            double angle = angle_between_centers(cx_purple, cy_purple, cx_blue, cy_blue);
            putText(result, to_string(angle), Point(cx_blue, cy_blue), FONT_HERSHEY_SIMPLEX, 1, Scalar(256, 256, 256), 2);
        }
    
        // Показываем результат на экране
        imshow("Original", frame);
        imshow("Filtered", result);

        // При нажатии на esc выходим
        if (waitKey(1) == 27)
            break;
        }
    
    // Освобождаем ресурсы и закрываем окна
    cap.release();
    destroyAllWindows();
}
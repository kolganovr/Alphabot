#include "ImageProcessing.h"

using namespace std;
using namespace cv;

tuple<Mat, Mat> Camera::getImage()
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

Camera::Camera()
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

Camera::~Camera()
{
    // Закрываем камеру
    cap.release();
    destroyAllWindows();
}

tuple<Mat, Mat> Camera::sendToServer()
{
    return getImage();
}

ThresholdGenerator::ThresholdGenerator() {}

void ThresholdGenerator::trackBar()
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

void ThresholdGenerator::saveHSVtoFile(vector<Scalar> hsvValues)
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

tuple<Scalar, Scalar, Scalar, Scalar, Scalar, Scalar> ThresholdGenerator::getHSVfromFile()
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

void ThresholdGenerator::recieveImage(const Mat &hsv, const Mat &frame)
{
    this->hsv = hsv;
    this->frame = frame;

    // Задаем маску
    inRange(hsv, Scalar(hMin, sMin, vMin), Scalar(hMax, sMax, vMax), mask);

    // Отображаем результаты
    imshow("Mask", mask);
}

tuple<Scalar, Scalar, Scalar, Scalar, Scalar, Scalar> ThresholdGenerator::sendHSVtoServer()
{
    return getHSVfromFile();
}

tuple<int, int, int, int, int, int> ThresholdGenerator::getHSV()
{
    return make_tuple(hMin, hMax, sMin, sMax, vMin, vMax);
}
#include <opencv2/opencv.hpp>

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

void Camera::changeCameraType()
{
    string statusOfConnection = "";
    isWebcam = !isWebcam;
    cap.release();
    if (isWebcam)
    {
        cap.open(0);
    }
    else
    {
        cout << "IP_CAM: " << IP_CAM << endl;
        if (!ipEntered)
        {
            cout << "Press 'c' in 5 seconds to edit IP or default ip will be used..." << endl;
            char ch = cv::waitKey(5000);

            if (ch == 'c')
            {
                cout << "Enter IP address: ";
                cin >> IP_CAM;
            }
            else
                cout << "Using default IP address: " << IP_CAM << endl;
        }
        // Попытаться подключиться к камере по IP, если ошибка, то внешняя камера не подключена
        cap.open(IP_CAM);
        if (!cap.isOpened())
        {
            cout << "IP camera not found" << endl;
            isWebcam = true;
            cap.open(0);
            statusOfConnection = "doesn't ";
        }
        // Если подключение успешно, то больше не нужно вводить IP адрес
        else
            ipEntered = true;
    }
    cout << "Camera type " << statusOfConnection << "changed" << endl;
}

void Camera::setIPcam(string ip)
{
    IP_CAM = ip;
}

bool Camera::getCameraType()
{
    return isWebcam;
}

ThresholdGenerator::ThresholdGenerator() {}

void ThresholdGenerator::trackBar()
{
    // reset HSV values
    lowerHSV = {0, 0, 0};
    upperHSV = {179, 255, 255};
    // Создаем hue reference image
    Mat hueRef(1, 179, CV_8UC3);
    for (int i = 0; i < 179; i++)
    {
        hueRef.at<Vec3b>(0, i) = Vec3b(i, 255, 255);
    }
    cvtColor(hueRef, hueRef, COLOR_HSV2BGR);
    resize(hueRef, hueRef, Size(160, 20));

    namedWindow("Threshold", WINDOW_NORMAL);
    // Создаем трекбары для настройки фильтра
    createTrackbar("Hue Min", "Threshold", &lowerHSV[0], 179);
    imshow("HUE", hueRef);
    createTrackbar("Hue Max", "Threshold", &upperHSV[0], 179);
    createTrackbar("Sat Min", "Threshold", &lowerHSV[1], 255);
    createTrackbar("Sat Max", "Threshold", &upperHSV[1], 255);
    createTrackbar("Val Min", "Threshold", &lowerHSV[2], 255);
    createTrackbar("Val Max", "Threshold", &upperHSV[2], 255);
}

void ThresholdGenerator::saveHSVtoFile(vector<Scalar> lowerHSV, vector<Scalar> upperHSV)
{
    ofstream file;
    file.open("../../docs/hsvValues.txt");

    if (!file.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }

    // Записываем в файл значения нижней и верхней границы цветов
    for (int i = 0; i < lowerHSV.size(); i++)
    {
        file << lowerHSV[i][0] << " " << lowerHSV[i][1] << " " << lowerHSV[i][2] << endl;
        file << upperHSV[i][0] << " " << upperHSV[i][1] << " " << upperHSV[i][2] << endl;
    }
    cout << "HSV values saved to file" << endl;
    file.close();
}

vector<vector<Scalar>> ThresholdGenerator::getHSVfromFile()
{
    ifstream file;
    file.open("../../docs/hsvValues.txt");

    if (!file.is_open())
    {
        cout << "Error opening file" << endl;
        return {};
    }

    vector<vector<Scalar>> hsvValues;
    vector<Scalar> lowerHSV;
    vector<Scalar> upperHSV;

    // Считываем значения из файла
    while (!file.eof())
    {
        int hMin, sMin, vMin, hMax, sMax, vMax;
        file >> hMin >> sMin >> vMin >> hMax >> sMax >> vMax;
        lowerHSV.push_back(Scalar(hMin, sMin, vMin));
        upperHSV.push_back(Scalar(hMax, sMax, vMax));
    }
    file.close();

    // Записываем значения в вектор
    hsvValues.push_back(lowerHSV);
    hsvValues.push_back(upperHSV);

    return hsvValues;
}

void ThresholdGenerator::recieveImage(const Mat &hsv, const Mat &frame)
{
    this->hsv = hsv;
    this->frame = frame;

    // Задаем маску
    inRange(hsv, Scalar(lowerHSV[0], lowerHSV[1], lowerHSV[2]),
            Scalar(upperHSV[0], upperHSV[1], upperHSV[2]), mask);

    // Отображаем результаты
    imshow("Mask", mask);
}

vector<vector<Scalar>> ThresholdGenerator::sendHSVtoServer()
{
    return getHSVfromFile();
}

tuple<int, int, int, int, int, int> ThresholdGenerator::getHSV()
{
    return make_tuple(lowerHSV[0], lowerHSV[1], lowerHSV[2], upperHSV[0], upperHSV[1], upperHSV[2]);
}
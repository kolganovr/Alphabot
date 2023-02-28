import cv2

def convert_hsv(hsv):
    # Разбиваем hsv на отдельные компоненты
    h, s, v = hsv
    # Переводим h из диапазона [0, 360] в диапазон [0, 180]
    h = int(h * 180 / 360)
    # Переводим s и v из диапазона [0, 100] в диапазон [0, 255]
    s = int(s * 255 / 100)
    v = int(v * 255 / 100)
    # Собираем новое значение hsv в кортеж
    hsv_new = (h, s ,v)
    # Возвращаем новое значение hsv
    return hsv_new

# Создаем объект для захвата видео с веб-камеры
cap = cv2.VideoCapture(0)

# Бесконечный цикл
while True:
    # Читаем кадр из видео
    ret, frame = cap.read()
    # Проверяем, что кадр успешно прочитан
    if ret:
        # Конвертируем кадр из BGR в HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        # Задаем границы для фиолетового цвета в HSV # hsl(339, 71%, 64%)
        lower_purple = convert_hsv((300, 30, 30))
        upper_purple = convert_hsv((350, 100, 100))

        # Применяем функцию inRange для фильтрации фиолетового цвета
        mask = cv2.inRange(hsv, lower_purple, upper_purple)
        
        # Накладываем маску на исходный кадр
        result = cv2.bitwise_and(frame, frame, mask=mask)
        # Показываем результат на экране
        cv2.imshow("Result", result)
        cv2.imshow("Frame", frame)
    # Ждем нажатия клавиши Esc для выхода из цикла
    if cv2.waitKey(1) == 27:
        break

# Освобождаем ресурсы и закрываем окна
cap.release()
cv2.destroyAllWindows()
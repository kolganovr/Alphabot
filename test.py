import cv2
import math

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

def angle_between_centers(cx_purple, cy_purple, cx_blue, cy_blue):
    # Проверки
    if cx_purple == 0 or cy_purple == 0 or cy_blue == 0 or cx_blue == 0:
        return 0
    if cy_purple-cy_blue == 0:
        return 90
    if cx_purple == cx_blue and cy_blue < cy_purple:
        return 180
    
    # Вычисляем тангенс угола между центрами
    slope = (cx_purple-cx_blue) / (cy_blue-cy_purple)
    
    angle_rad = math.atan(slope)
    angle_deg = math.degrees(angle_rad)

    if angle_deg < 0 and cy_blue < cy_purple:
        angle_deg = 180 + angle_deg
    elif cx_blue > cx_purple and cy_blue < cy_purple:
        angle_deg = -180 + angle_deg

    # Возвращаем с точностью до 2х знаков после запятой
    return round(angle_deg, 2)

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

        # Задем границы для синего двета в HSV hsl(200, 79%, 49%)
        lower_blue = convert_hsv((180, 50, 30))
        upper_blue = convert_hsv((220, 100, 100))

        # Применяем функцию inRange для фильтрации фиолетового и синего цвета
        maskPurple = cv2.inRange(hsv, lower_purple, upper_purple)
        maskBlue = cv2.inRange(hsv, lower_blue, upper_blue)

        mask = cv2.bitwise_or(maskBlue, maskPurple)
        # Накладываем маску на исходный кадр
        result = cv2.bitwise_and(frame, frame, mask=mask)

        cx_purple = 0
        cy_purple = 0
        cx_blue = 0
        cy_blue = 0
        # Поиск контуров объектов на маске с фильтрованным фиолетовым цветом
        contoursPurple, _ = cv2.findContours(maskPurple, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        # Если найден хотя бы один контур
        if len(contoursPurple) > 0:
            # Находим контур с максимальной площадью
            max_contour = max(contoursPurple, key=cv2.contourArea)
            # Находим координаты центра масс контура
            M = cv2.moments(max_contour)
            if M["m00"] != 0:
                cx_purple = int(M["m10"] / M["m00"])
                cy_purple = int(M["m01"] / M["m00"])
                # Рисуем маленький красный круг в центре масс контура
                cv2.circle(result, (cx_purple, cy_purple), 10, (0, 0, 255), -1)

        # Поиск контуров объектов на маске с фильтрованным синим цветом
        contoursBlue, _ = cv2.findContours(maskBlue, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        # Если найден хотя бы один контур
        if len(contoursBlue) > 0:
            # Находим контур с максимальной площадью
            max_contour = max(contoursBlue, key=cv2.contourArea)
            # Находим координаты центра масс контура
            M = cv2.moments(max_contour)
            if M["m00"] != 0:
                cx_blue = int(M["m10"] / M["m00"])
                cy_blue = int(M["m01"] / M["m00"])
                # Рисуем маленький красный круг в центре масс контура
                cv2.circle(result, (cx_blue, cy_blue), 10, (256, 0, 0), -1)

        if len(contoursPurple) > 0 and len(contoursBlue) > 0:
            # Рисуем вертикальную белую линию через центр масс синего цвета
            cv2.line(result, (cx_blue, cy_blue), (cx_blue, 0), (256, 256, 256), 1)

            # Рисуем линию соединяющую оба центра масс
            cv2.line(result, (cx_purple, cy_purple), (cx_blue, cy_blue), (256, 256, 256), 1)

            # Выведим на экран угол между линиями
            angle = angle_between_centers(cx_purple, cy_purple, cx_blue, cy_blue)
            cv2.putText(result, str(angle), (cx_blue, cy_blue), cv2.FONT_HERSHEY_SIMPLEX, 1, (256, 256, 256), 2)
    # Показываем результат на экране
    cv2.imshow("Result", result)
    cv2.imshow("Frame", frame)

    # Ждем нажатия клавиши Esc для выхода из цикла
    if cv2.waitKey(1) == 27:
        break

# Освобождаем ресурсы и закрываем окна
cap.release()
cv2.destroyAllWindows()
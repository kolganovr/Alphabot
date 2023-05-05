import json
import os

# Получаем абсолютный путь к файлу settings.py
absolute_path = os.path.dirname(__file__)

# Удаляем 4 последних символа в пути, чтобы получить путь к корню проекта
absolute_path = absolute_path[:-4]
absolute_path = absolute_path.replace("\\", "/")

hsvValuesJSON_path = absolute_path + "/Settings/hsvValues.json"
hsvValuesTXT_path = absolute_path + "/.temp/hsvValues.txt"

# Открываем файл с данными о границах цветов
with open(hsvValuesTXT_path, 'r') as f:
    # Считываем данные из файла
    data = f.read()

# Преобразуем строки в массивы целых чисел
data = [list(map(int, line.strip().split())) for line in data.split('\n') if line.strip()]

# Создаем пустой объект, который мы заполним данными
colors = {
    "red": {
        "low": {},
        "high": {}
    },
    "green": {
        "low": {},
        "high": {}
    },
    "blue": {
        "low": {},
        "high": {}
    }
}

# Заполняем объект данными о границах цветов
colors["red"]["low"]["H"], colors["red"]["low"]["S"], colors["red"]["low"]["V"] = data[0]
colors["red"]["high"]["H"], colors["red"]["high"]["S"], colors["red"]["high"]["V"] = data[1]
colors["green"]["low"]["H"], colors["green"]["low"]["S"], colors["green"]["low"]["V"] = data[2]
colors["green"]["high"]["H"], colors["green"]["high"]["S"], colors["green"]["high"]["V"] = data[3]
colors["blue"]["low"]["H"], colors["blue"]["low"]["S"], colors["blue"]["low"]["V"] = data[4]
colors["blue"]["high"]["H"], colors["blue"]["high"]["S"], colors["blue"]["high"]["V"] = data[5]

# Сохраняем объект в файл .json
with open(hsvValuesJSON_path, 'w') as f:
    json.dump(colors, f, indent=4)
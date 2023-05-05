import json
import os

# Получаем абсолютный путь к файлу settings.py
absolute_path = os.path.dirname(__file__)

# Удаляем 4 последних символа в пути, чтобы получить путь к корню проекта
absolute_path = absolute_path[:-4]
absolute_path = absolute_path.replace("\\", "/")

confJSON_path = absolute_path + "/Settings/config.json"
configTXT_path = absolute_path + "/.temp/config.txt"

hsvValuesJSON_path = absolute_path + "/Settings/hsvValues.json"
hsvValuesTXT_path = absolute_path + "/.temp/hsvValues.txt"

# Проверяем, существует ли папка .temp
if not os.path.exists(absolute_path + "/.temp"):
    # Если не существует, то создаем ее
    os.mkdir(absolute_path + "/.temp")

# Открываем входной JSON файл с настройками
with open(confJSON_path, 'r') as f:
    # Загружаем данные JSON
    data = json.load(f)

# Открываем выходной текстовый файл
with open(configTXT_path, 'w') as f:
    # Проходим по каждому свойству в данных JSON
    for key in data:
        # Записываем значение свойства в текстовый файл
        f.write(str(data[key]) + '\n')

# Открываем входной JSON файл с настройками HSV
with open(hsvValuesJSON_path, 'r') as f:
    # Загружаем данные JSON
    data = json.load(f)

with open(hsvValuesTXT_path, 'w') as f:
    for color in data:
        f.write(f"{data[color]['low']['H']} {data[color]['low']['S']} {data[color]['low']['V']}\n")
        f.write(f"{data[color]['high']['H']} {data[color]['high']['S']} {data[color]['high']['V']}\n")
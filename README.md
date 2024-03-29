# Общая информация
Проект для робота Alphabot по нахождению и "стиранию" граффити

Выполнили:

**Колганов Р. А.**

**Мантлер Н. К.**
# Использование
## Требования
Python: `2.7.9+` или `3.6+`

Также необходимо установить `mosquitto` для работы с протоколом MQTT. Для этого воспользуйтесь иструкцией на сайте [mosquitto](https://mosquitto.org/download/).

После установки **обязательно** добавьте путь к исполняемому файлу `mosquitto` в переменную окружения `PATH`.

Для связи с альфаботом необходимо **отключить брэндмауэр** на компьютере, на котором запускается программа, а также отключить VPN
## Запуск программы
В папке `/Settings` в корневой директории проекта находится файл `config.json`. В этом файле необходимо указать:
1. IP-адрес внещней камеры
2. Режим вывода изображения (`true` - выводить изображение с камеры и результаты поиска, `false` - не выводить)

Для запуска программы необходимо собрать и запустить файл `main.cpp` в корневой директории проекта.
## Во время работы программы
Программа автоматически находит цветовые маркеры находящиеся на роботе и граффити и отображает их в окне. Программа рисует линию, соединяющую центр робота и центр граффити. Также программа выводит на экран угол между роботом и граффити. В консоли выводится информация о действии, которое необходимо совершить роботу, чтобы приблизиться к граффити. По протоколу MQTT программа отправляет сообщение с информацией о действии, которое необходимо совершить роботу.

## Установка порогов
При нажатии на `space`, программа переключается в режим установки порогов. В этом режиме программа отображает изображение с камеры с текущими значениями порогов, установленных с помощью трекбаров. При повторном нажатии на `space`, программа переключается обратно в режим поиска граффити. Если какой-либо маркер не находится с помощью программы, то можно установить пороги вручную с помощью трекбаров. После установки порога нужно выбрать к какому маркеру применить текущую маску. Для этого нужно нажать на соответствующую кнопку.
* `1` - маркер фиолетового цвета (нос робота)
* `2` - маркер зеленого цвета (граффити)
* `3` - маркер синего цвета (хвост робота)

В последней строчке можно задать время выполнения роботом каждой команды (0-10), где 10 - 1 секунда. Значение по умолчанию - 1 (0.1 секунды)

После применения изменений порогов, можно нажать `space` для проверки результатов. Если результаты удовлетворительны, то можно сохранить пороги в файл с помощью кнопки `s`, находясь в режиме установки порогов. JSON файл с настройками будет сохранён в папке `/Settings` в проекте с именем `hsvValues.json`.

Если такой файл уже создан и в нём уже есть настройки, то, находясь в режиме установки порогов, можно загрузить настройки из файла с помощью кнопки `L`.

# Описание алгоритма
## Описание работы программы
Программа состоит из двух основных частей: `Alphabot` и `Server`. `Alphabot` управляет движением робота, а `Server` отвечает за поиск граффити и робота, а также за определение действия которое необходимо совершить роботу.

1. `Camera` получает изображение и передает его `Server`
2. `Server` обрабатывает изображение и ищет на нем граффити и робота. Если найдены, то `Server` высчитывает, какое действие необходимо совершить роботу, чтобы приблизиться к граффити.
3. `Server` передает действие `Alphabot` по протоколу MQTT
4. `Alphabot` с помощью одного из классов `EngineController` или `Cleaner` выполняет поставленное действие.

## Схема КТС
![Схема КТС](https://github.com/kolganovr/Alphabot/raw/main/docs/KTC.png)

## Class diagram
![Class diagram](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/dev/docs/plantUML.wsd#3)

## Sequence diagram
Примерная диаграмма работы программы

![Sequence diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/dev/docs/sequence_diagram.wsd#3)

## Status diagram
Диаграмма состояний для Альфабота. Состояния изменяются в методе Server::chooseAction()

![Status diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/dev/docs/status_diagram.wsd)
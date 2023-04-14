# Общая информация
Проект для робота Alphabot по нахождению и "стиранию" граффити

Выполнили:

**Колганов Р.А.**

**Мантлер Н.К**
# Использование
## Запуск программы
Для запуска программы необходимо собрать и запустить файл `main.cpp` в корневой директории проекта.

## Во время работы программы
Программа автоматически находит цветовые маркеры находящиеся на роботе и граффити и отображает их на двух отдельных окнах. Программа рисует линию, соединяющую центр робота и центр граффити. Также программа выводит на экран угол между роботом и граффити. В консоли выводится информация о действии которое необходимо совершить роботу, чтобы приблизиться к граффити.

При нажатии на кнопку `с` программа переключается на получение изображения с IP-камеры. При первом переключении за запуск программы она спрашивает IP-адрес камеры. После этого программа запоминает IP-адрес и больше не спрашивает его. Если IP-адрес камеры не доступен, то программа выведет сообщение об ошибке и продолжит работу со встроенной камерой.

## Установка порогов
При нажатии на `space` программа переключается в режим установки порогов. В этом режиме программа отображает изображение с камеры с текущими значениями порогов, установленных с помощью трекбаров. При нажатии на `space` программа переключается обратно в режим поиска граффити. Если какой-либо маркер не находится с помощью программы, то можно установить пороги вручную с помощью трекбаров. После установки порога нужно выбрать к какому маркеру применить текущую маску. Для этого нужно нажать на соответствующую кнопку.
* `1` - маркер фиолетового цвета
* `2` - маркер зеленого цвета
* `3` - маркер синего цвета

После применения изменений порогов можно нажать `space` для проверки результатов. Если результаты удовлетворительны, то можно сохранить пороги в файл с помощью кнопки `s` находясь в режиме установки порогов. Текстовый файл с настройками будет сохранён в папке `/docs` в проекте с именем `hsvValues.txt`.

Если такой файл уже создан и в нём уже есть настройки, то находясь в режиме установки порогов можно загрузить настройки из файла с помощью кнопки `l`.

# Описание алгоритма
## Описание работы программы
Программа состоит из двух основных частей: `Alphabot` и `Server`. `Alphabot` управляет движением робота, а `Server` отвечает за поиск граффити и робота, а также за определение действия которое необходимо совершить роботу.

1. `Camera` получает изображение и передает его `Server`
2. `Sevrer` обрабатывает изображение и ищет на нем граффити и робота. ЕСли найдены, то `Server` высчитывает какое действие необходимо совершить роботу, чтобы приблизиться к граффити.
3. `Server` передает действие `Alphabot`
4. `Alpabot` с помощью одного из классов `EngineController` или `Cleaner` выполняет поставленное действие.

## Схема КТС
![Схема КТС](https://github.com/kolganovr/Alphabot/raw/main/docs/KTC.png)

## Class diagram
![Class diagram](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/main/plantUML.wsd#1)

## Sequence diagram
Примерная диграмма работы программы

![Sequence diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/dev/docs/sequence_diagram.wsd)

## Status diagram
Диаграмма состояний для Алфабота. Состояния изменяются в методе Server::chooseAction()

![Status diagram](https://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/dev/docs/status_diagram.wsd)
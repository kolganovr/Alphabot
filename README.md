# Использование
## Запуск программы
Для запуска программы необходимо собрать и запустить файл `main.cpp` в корневой директории проекта.

## Во время работы программы
Программа автоматически находит цветовые маркеры находящиеся на роботе и граффити и отображает их на двух отдельных окнах. Программа рисует линию, соединяющую центр робота и центр граффити. Также программа выводит на экран угол между роботом и граффити.

## Установка порогов
При нажатии на `space` программа переключается в режим установки порогов. В этом режиме программа отображает изображение с камеры с текущими значениями порогов, установленных с помощью трекбаров. При нажатии на `space` программа переключается обратно в режим поиска граффити. Если какой-либо маркер не находится с помощью программы, то можно установить пороги вручную с помощью трекбаров. После установки порога нужно выбрать к какому маркеру применить текущую маску. Для этого нужно нажать на соответствующую кнопку.
* `1` - маркер фиолетового цвета
* `2` - маркер зеленого цвета
* `3` - маркер синего цвета

После применения изменений порогов можно нажать `space` для проверки результатов. Если результаты удовлетворительны, то можно сохранить пороги в файл с помощью кнопки `s` находясь в режиме установки порогов. Текстовый файл с настройками будет сохранён в папке `/docs` в проекте с именем `hsvValues.txt`.

Если такой файл уже создан и в нём уже есть настройки, то находясь в режиме установки порогов можно загрузить настройки из файла с помощью кнопки `l`.
# Описание кода
Проект для робота Alphabot по нахождению и "стиранию" граффити

Выполнили:

**Колганов Р.А.**

**Мантлер Н.К**

## Классы:
### EngineController
Класс, который управляет движением двигателя. У него есть четыре метода для перемещения двигателя вперед, назад, влево и вправо, с указанным количеством времени.

### Cleaner
Класс, который выполняет операцию очистки.

### Camera
Класс, который управляет камерой. У него есть метод `getImage`, который получает изображение с камеры, переводит его в формат HSV и возвращает его. Также есть метод `sendToServer`, который отправляет изображение на сервер.

### Graffiti
Класс, представляющий объект граффити. У него два атрибута `posX` и `posY`, представляющие позицию объекта на оси x и y соответственно. Также есть булевый атрибут `exist`, указывающий, существует ли объект или нет. Класс имеет конструктор, который принимает `x` и `y` в качестве аргументов и инициализирует атрибуты объекта соответствующим образом. В классе также есть три метода-геттера для получения позиций объекта `x` и `y`, и `exist`. Также есть метод `setState` для изменения состояния объекта.

### Alphabot
Класс, который управляет движением робота. У него есть атрибут `engineStarted`, указывающий, запущен ли двигатель или нет. У него также есть экземпляр `EngineController`, `Cleaner` и объект `Graffiti`. Alphabot имеет координаты `posX` и `posY`, и угол относительно цели `angle`. Класс имеет методы для запуска и остановки двигателя, метод `chooseAction` для выбора направления движения и метод `doAction` для выполнения действия, такого как движение вперед, назад, влево или вправо, или выполнение операции очистки. Также есть метод `resieveMessage`, который получает сообщение от сервера и устанавливает соответствующие атрибуты объекта. Метод 'SetPosition' устанавливает координаты робота. Есть два геттера для каждой позиции `getPosX` и `getPosY`.

### ThresholdGenerator
Класс, который генерирует пороги для HSV изображения во время выполнения программы. У него есть метод `trackBar` для создания трекбаров. Есть метод для получения изображения с камеры и отображения маски `recieveImage`, а также метод `sendHSVtoServer`, который отправляет HSV маски на сервер читая их из файла с помощью `getHSVfromFile`. Есть и метод для сохранения HSV маски в файл `saveHSVtoFile`.

### Server
Класс, который управляет сервером. У него есть метод `recieveMessage`, который получает изображение с камеры, ищет граффити с помощью `searchForGraffiti`, вычисляет координаты робота методом `searchForRobot`, отображает картинку методом `showResults` и отправляет сообщение роботу методом `sendMessage`. С помощью `AlphabotAngleCalc` сервер вычисляет угол между роботом и граффити. `drawGraphics` отрисовывает маркеры на изображении. `sendHSVToThresholdGenerator` отправляет текущие значения маски генератору маски для их сохранения. `recieveHSVFromThresholdGenerator` получает значения маски от генератора маски и устанавливает их в качестве текущих. `setDebugMode` переключается между режимом установки маски и активным режимом поиска. `setHSV` устанавливает значения маски.

## Enum
### Action
Перечисление, которое представляет возможные действия робота. Оно содержит следующие константы:
* `FORWARD`: двигаться вперед
* `BACKWARD`: двигаться назад
* `LEFT`: повернуть налево
* `RIGHT`: повернуть направо
* `CLEAN`: выполнить операцию очистки

Константы этого перечисления могут использоваться для выбора действий в методе `chooseAction` класса `Alphabot`.

# Описание алгоритма
## Описание работы программы
Программа состоит из двух основных частей: `Alphabot` и `Server`. `Alphabot` управляет движением робота, а `Server` отвечает за поиск граффити и отправку данных на сервер.

1. `Camera` получает изображение и передает его `Server`
2. `Sevrer` обрабатывает изображение и ищет на нем граффити и робота. В случае если граффити найдено создается объект класса `Graffiti` и передается `Alphabot`'у
3. `Alphabot` определяет в какую ему сторону необходимо двигаться с помощью метода `chooseAction` и отправляет свое решение в `doAction`
4. `doAction` с помощью одно из классов `EngineController` или `Cleaner` выполняет поставленное действие

## Схема КТС
![Схема КТС](https://github.com/kolganovr/Alphabot/raw/main/docs/KTC.png)

## UML
![UML](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/main/plantUML.wsd#1)
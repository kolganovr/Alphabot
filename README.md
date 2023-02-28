# Описание кода
Проект для робота Alphabot по нахождению и "стиранию граффити"

Выполнили:

**Колганов Р.А.**

**Мантлер Н.К**

## Классы:
### EngineController
Класс, который управляет движением двигателя. У него есть четыре метода для перемещения двигателя вперед, назад, влево и вправо, с указанным количеством времени.

### Cleaner
Класс, который выполняет операцию очистки.

### Camera
Класс, который отправляет данные на сервер.

### Graffiti
Класс, представляющий объект граффити. У него два атрибута `posX` и `posY`, представляющие позицию объекта на оси x и y соответственно. Также есть булевый атрибут `exist`, указывающий, существует ли объект или нет. Класс имеет конструктор, который принимает `x` и `y` в качестве аргументов и инициализирует атрибуты объекта соответствующим образом. В классе также есть два метода-геттера для получения позиций объекта `x` и `y`.

### Server
Класс, который имеет метод `searchForGraffiti`, который возвращает объект `Graffiti`, если он найден, и методы для отправки и получения сообщений от сервера.

### Alphabot
Класс, который управляет движением робота. У него есть атрибут `engineStarted`, указывающий, запущен ли двигатель или нет. У него также есть экземпляр `EngineController`, `Cleaner` и объект `Graffiti`. Класс имеет методы для запуска и остановки двигателя, метод `chooseAction` для выбора направления движения и метод `doAction` для выполнения действия, такого как движение вперед, назад, влево или вправо, или выполнение операции очистки.

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
2. `Sevrer` обрабаотывает изображение и ищет на нем граффити. В случае если граффити найдено создается обьект класса `Graffiti` и передается `Alphabot`'у
3. `Alphabot` определяет в какую ему сторону необходимо двигаться с помощью метода `chooseAction` и отправляет свое решение в `doAction`
4. `doAction` с помощью одно из классов `EngineController` или `Cleaner` выполняет поставленное действие

## Схема КТС
![Схема КТС](https://github.com/kolganovr/Alphabot/raw/main/docs/KTC.png)

## UML
![UML](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/kolganovr/Alphabot/main/plantUML.wsd)
#pragma once

// Класс обьъекта граффити
class Graffiti
{
private:
    bool exist = false; // Существование граффити
    int posX;           // X позиция граффити
    int posY;           // Y позиция граффити
public:
    Graffiti();
    Graffiti(int x, int y);

    /// Устанавливает позицию граффити
    /// @param x X позиция
    /// @param y Y позиция
    void setState(int x, int y, bool exists);

    /// Возвращает X позицию граффити, если существует, иначе -1
    /// @return Позиция X -- 0 Левая сторона
    int getPosX();

    /// Возвращает Y позицию граффити (если существует, иначе -1)
    /// @return Позиция Y -- 0 верхняя сторона
    int getPosY();
    
    /// Возвращает существование граффити
    /// @return true если граффити существует
    bool isExist();
};
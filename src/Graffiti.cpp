#include "Graffiti.h"

Graffiti::Graffiti()
{
}

Graffiti::Graffiti(int x, int y)
{
    posX = x;
    posY = y;
    exist = true;
}

void Graffiti::setState(int x, int y, bool exists)
{
    if (x < 0 || y < 0)
    {
        exist = false;
        return;
    }
    posX = x;
    posY = y;
    exist = exists;
}

int Graffiti::getPosX() { return exist ? posX : -1; }

int Graffiti::getPosY() { return exist ? posY : -1; }

bool Graffiti::isExist() { return exist; }
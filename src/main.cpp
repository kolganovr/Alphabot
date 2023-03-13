#include <iostream>

#include "Server.h"
#include "Alphabot.h"
#include "Graffiti.h"
#include "ImageProcessing.h"
#include "KeyParser.h"

int main()
{
    Server server;
    KeyParser keyParser(&server);

    while (true)
    {
        if (!keyParser.parseKeys())
        {
            break;
        }
    }
    return 0;
}
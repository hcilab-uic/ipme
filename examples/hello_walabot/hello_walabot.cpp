#include <iostream>
#include <memory>
#include <sstream>

#include "walabot.h"

int main()
{
    using Settings = cdi::sensor::Walabot::Settings;
    Settings settings{Settings::Inner{30, 200, 3}, Settings::Inner{-15, 15, 5},
                      Settings::Inner{-60, 60, 5}};
    cdi::sensor::Walabot walabot{settings, true};
    walabot.record(10);
}

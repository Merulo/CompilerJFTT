#include "EmulatorRunner.hpp"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>

bool EmulatorRunner::emulate(std::string tmpFileName, std::string resultFileName)
{
    if (!fileExists("emulator.py"))
    {
        return false;
    }

    std::string command = "python3 emulator.py " + tmpFileName + " " + resultFileName + "> /dev/null 2> /dev/null";
    int v = system(command.c_str());
    return v == 0;
}

bool EmulatorRunner::fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return (bool)file;
}
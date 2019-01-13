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

    std::string command = "python emulator.py " + tmpFileName + " " + resultFileName;
    int v = system(command.c_str());
    return v == 0;
}

bool EmulatorRunner::fileExists(const std::string& filename) {
    std::ifstream file(filename.c_str());
    return (bool)file;
}
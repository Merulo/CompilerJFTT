#pragma once

#include "string"

struct Register
{
    unsigned long long value = 0;
    std::string variableName;
    std::string registerName;
    bool isUsed = false;
};
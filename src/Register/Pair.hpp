#pragma once

#include "DataTypes/Block.hpp"
#include "RegisterBlock.hpp"

struct Pair
{
    Block block;
    RegisterBlock endRegisterBlock;
    RegisterBlock startRegisterBlock;
    bool registerBlockIsSet = false;
};

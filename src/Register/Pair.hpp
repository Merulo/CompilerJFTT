#pragma once

#include "DataTypes/Block.hpp"
#include "RegisterBlock.hpp"

struct Pair
{
    Block block;
    RegisterBlock registerBlock;
    RegisterBlock startRegisterBlock;
    bool registerBlockIsSet = false;
};

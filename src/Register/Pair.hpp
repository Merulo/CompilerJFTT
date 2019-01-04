#pragma once

#include "DataTypes/Block.hpp"
#include "RegisterBlock.hpp"

struct Pair
{
    Block block;
    RegisterBlock registerBlock;
    bool registerBlockIsSet = false;
};

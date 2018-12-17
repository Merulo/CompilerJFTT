#pragma once

#include <iostream>

#include "SymbolTable.hpp"
#include "ThreeAddressCode.hpp"

class Driver
{
    public:
    SymbolTable ST;
    ThreeAddressCode TAC;

    void test()
    {
        std::cerr<<"Hello world!"<<std::endl;
    }
    


    private:
    
};
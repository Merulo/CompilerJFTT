#include <iostream>

#include "SymbolTable.hpp"
#include "ThreeAddressCode.hpp"

class Driver
{
    public:
    SymbolTable symbolTable;
    ThreeAddressCode threeAddressCode;

    void test()
    {
        std::cerr<<"Hello world!"<<std::endl;
    }
    


    private:
    
};
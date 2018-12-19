#pragma once

#include <iostream>

#include "SymbolTable.hpp"
#include "IRs/ThreeAddressCode.hpp"
#include "IRs/FinalIR.hpp"

class Driver
{
    public:
    SymbolTable ST;
    ThreeAddressCode TAC;

    void test()
    {
        std::cerr<<"Hello world!"<<std::endl;
    }
    void compile()
    {
        //memory assigned
        ST.assignMemory();
        ST.print();
        TAC.print();
        //change TAC to finalIR

        //change finalIR to assembler
        


    }
    


    private:
    
};
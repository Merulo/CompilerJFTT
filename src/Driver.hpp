#pragma once

#include <iostream>

#include "SymbolTable.hpp"
#include "IRs/ThreeAddressCode.hpp"
#include "IRs/FinalIR.hpp"
#include "RegisterAllocator.hpp"

class Driver
{
    public:
    SymbolTable ST;
    ThreeAddressCode TAC;
    FinalIR FIR;

    void test()
    {
        std::cerr<<"Hello world!"<<std::endl;
    }
    //this means that first TAC was created
    void compile(const std::string& fileName)
    {
        //loop over TAC to and transform to optimize

        //memory assigned
        ST.assignMemory();
        
        //change TAC to finalIR
        FIR.parse(TAC.getLines());

        //change finalIR to assembler
        RegisterAllocator RA(ST);
        RA.compile(fileName, FIR.getLines());

        //print for test purposes
        // ST.print();
        // TAC.print();
        // FIR.print();
        RA.print();
    }
    


    private:
    
};
#pragma once

#include <iostream>

#include "SymbolTable.hpp"
#include "IRs/FirstIR.hpp"

class Driver
{
    public:
    SymbolTable ST;
    FirstIR FIR;
    // FinalIR FIR;

    void test()
    {
        std::cerr<<"Hello world!"<<std::endl;
    }
    //this means that first FIR was created
    void compile(const std::string& fileName)
    {
        //loop over FIR to and transform to optimize

        //transfer const ownership
        ST.setConsts(FIR.getConsts());
        //memory assigned
        ST.assignMemory();
        
        // //change FIR to finalIR
        // FIR.parse(FIR.getLines());

        // //change finalIR to assembler
        // RegisterAllocator RA(ST);
        // RA.compile(fileName, FIR.getLines());

        //print for test purposes
        // ST.print();
        // FIR.print();
        // FIR.print();
        // RA.print();
    }
    


    private:
    
};
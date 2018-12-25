#pragma once

#include <iostream>
#include <memory>

#include "SymbolTable.hpp"
#include "IRs/FirstIR.hpp"
#include "IRs/SecondIR.hpp"

class Driver
{
    public:
    Driver()
    {
        ST = std::make_shared<SymbolTable>();
    }
    std::shared_ptr<SymbolTable> ST;
    std::shared_ptr<SecondIR> SIR;
    FirstIR FIR;

    void convertToSIR()
    {
        SIR = std::make_shared<SecondIR>();
        SIR->parse(FIR.getBlocks());
    }

    //this means that first FIR was created
    void compile(const std::string& fileName)
    {
        ST->assignMemory();
    }
    


    private:
    
};
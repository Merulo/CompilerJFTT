#pragma once

#include <iostream>
#include <memory>

#include "SymbolTable.hpp"
#include "IRs/FirstIR.hpp"
#include "IRs/SecondIR.hpp"
#include "IRs/ThirdIR.hpp"

class Driver
{
    public:
    Driver()
    {
        FIR = std::make_shared<FirstIR>();
        ST = std::make_shared<SymbolTable>();
    }
    std::shared_ptr<SymbolTable> ST;
    std::shared_ptr<SecondIR> SIR;
    std::shared_ptr<ThirdIR> TIR;
    std::shared_ptr<FirstIR> FIR;

    void convertToSIR()
    {
        SIR = std::make_shared<SecondIR>();
        SIR->parse(FIR->getBlocks());
    }

    void convertToTIR()
    {
        TIR = std::make_shared<ThirdIR>();
        TIR->parse(SIR->getBlocks());
    }

    //this means that first FIR was created
    void compile(const std::string& fileName)
    {
        ST->assignMemory();
    }
    


    private:
    
};
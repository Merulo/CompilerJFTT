#pragma once

#include <iostream>
#include <memory>

#include "SymbolTable.hpp"
#include "IRs/FirstIR.hpp"
#include "IRs/SecondIR.hpp"
#include "IRs/ThirdIR.hpp"
#include "IRs/FourthIR.hpp"
#include "IRs/FifthIR.hpp"

class Driver
{
    public:
    Driver()
    {
        FIR = std::make_shared<FirstIR>();
        ST = std::make_shared<SymbolTable>();
    }
    std::shared_ptr<SymbolTable> ST;
    std::shared_ptr<FirstIR> FIR;
    std::shared_ptr<SecondIR> SIR;
    std::shared_ptr<ThirdIR> TIR;
    std::shared_ptr<FourthIR> FOIR;
    std::shared_ptr<FifthIR> FIIR;

    void convertToSIR()
    {
        SIR = std::make_shared<SecondIR>();
        SIR->setSymbolTable(ST);
        SIR->parse(FIR->getBlocks());
    }

    void convertToTIR()
    {
        TIR = std::make_shared<ThirdIR>();
        TIR->setSymbolTable(ST);
        TIR->parse(SIR->getBlocks());
    }

    void convertToFOIR(std::string fileName, bool useEmulator)
    {
        FOIR = std::make_shared<FourthIR>();
        FOIR->setSymbolTable(ST);
        FOIR->parse(TIR->getBlocks(), fileName, useEmulator);
    }

    void converToFIIR()
    {
        FIIR = std::make_shared<FifthIR>();
        FIIR->parse(FOIR->getBlocks());
    }
    


    private:
    
};
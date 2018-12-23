#pragma once

#include <iostream>
#include <memory>

#include "SymbolTable.hpp"
#include "IRs/FirstIR.hpp"

class Driver
{
    public:
    Driver()
    {
        ST = std::make_shared<SymbolTable>();
    }
    std::shared_ptr<SymbolTable> ST;

    // SymbolTable ST;
    FirstIR FIR;

    //this means that first FIR was created
    void compile(const std::string& fileName)
    {
        ST->assignMemory();
    }
    


    private:
    
};
#include "RegisterBlock.hpp"

RegisterBlock::RegisterBlock(std::shared_ptr<SymbolTable> symbolTable)
{
    _symbolTable = symbolTable;
    _currentRegister = 0;
}

void RegisterBlock::createRegisters()
{
    _addressRegister.name = "A";
    for(size_t i = 1; i < 8; i++){
        Register r;
        char c =  'A' + i;
        r.name = c;
        _registers.push_back(r);
    }
}

void RegisterBlock::exitBlock(Block& b)
{
    if (_registers[0].state != RegisterState::UNKNOWN)
    {
        saveToMemory(b, _registers[0], _registers[1]);

    }
    for(auto& r : _registers)
    {
        r.state = RegisterState::UNKNOWN;
        r.variableName = "";
    }
    _addressRegister.state = RegisterState::UNKNOWN;
    _addressRegister.variableName = "";
}

void RegisterBlock::print()
{
    std::cerr<<_addressRegister<<std::endl;
    for(auto& r : _registers)
    {
        std::cerr<<r<<std::endl;
    }
}

Register& RegisterBlock::getRegistersForOperation(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> usedRegisters)
{
    for(auto& reg : _registers)
    {
        if (std::find_if(usedRegisters.begin(), usedRegisters.end(), [reg](auto i)
            {
                return i.get() == reg;
            }) == usedRegisters.end())
        {
            saveToMemory(b, reg, _registers[0]);
            return reg;
        }
    }
    return _registers.back();
}

Register& RegisterBlock::getRegister(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> usedRegisters, bool load)
{
    b.lines.push_back({"#TEST = " + _registers[0].variableName});

    if (_symbolTable->isConst(name))
    {
        for(auto it = _registers.rbegin(); it != _registers.rend(); it++)
        {
            if (it->state == RegisterState::CONST && it->variableName == name)
            {
                return *it;
            }
        }

        for(auto it = _registers.rbegin(); it != _registers.rend(); it++)
        {
            if (it->state == RegisterState::UNKNOWN)
            {
                return *it;
            }
        }
    }

    if (_registers[0].variableName == name)
    {
        return _registers[0];
    }

    saveToMemory(b, _registers[0], _registers[1]);

    if (load)
    {
        loadFromMemory(b, name, _registers[0], _registers[1]);
    }

    return _registers[0];
}

Register& RegisterBlock::getSecondRegister(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> usedRegisters)
{
    for(auto it = _registers.rbegin(); it != _registers.rend(); it++)
    {
        if (it->variableName == name && std::find_if(usedRegisters.begin(), usedRegisters.end(), [it](auto i)
            {
                return i.get() == *it;
            }) == usedRegisters.end())
        {
            return *it;
        }
    }

    b.lines.push_back({"#getting second for " + name + " " + _registers[1].name});
    std::cout<<"getting second for "<<name<<" "<<_registers[1]<<std::endl;
    if (_registers[1].variableName == name)
    {
        return _registers[1];
    }
    b.lines.push_back({"#2getting second for " + name + " " + _registers[1].name});

    loadFromMemory(b, name, _registers[1], _registers[2]);

    return _registers[1];
}

std::vector<Line> RegisterBlock::generateNumber(unsigned long long& firstNumber, unsigned long long second, Register& usedRegister)
{
    if (firstNumber > second)
    {
        if (firstNumber - second < 15)
        {
            std::vector<Line> lines;
            while(firstNumber > second)
            {
                lines.push_back({"DEC", "A"});
                firstNumber--;
            }
            return lines;
        }
        else
        {         
            return generateNumberShift(firstNumber - second, usedRegister, "SUB");     
        }
    }
    else if (firstNumber < second )
    {
        if (second - firstNumber < 15)
        {
            std::vector<Line> lines;
            while( firstNumber < second)
            {
                lines.push_back({"INC", "A"});
                firstNumber++;
            }
            return lines;
        }
        else
        {         
            return generateNumberShift(second - firstNumber, usedRegister, "ADD"); 
        }
    }
    return {{"#ERROR"}};
}

std::vector<Line> RegisterBlock::generateNumberShift(unsigned long long value, Register& freeRegister, std::string operation)
{
    auto linesToAdd = NumberGenerator::generateConstFrom(value, {{freeRegister.name, 0}});
    linesToAdd.push_back({operation, "A", freeRegister.name});
    _addressRegister.state = RegisterState::UNKNOWN;    
    return linesToAdd;
}

/***************************************/
// SAVING TO MEMORY SECTION
/***************************************/

void RegisterBlock::saveToMemory(Block& b, Register& r, Register& freeRegister)
{
    if (r.state == RegisterState::VARIABLE && !r.variableName.empty())
    {
        saveVariableToMemory(b, r, freeRegister);
    }
    else if (r.state == RegisterState::TABLE)
    {
        std::string rest = r.variableName.substr(r.variableName.find("(") + 1, std::string::npos);
        rest.pop_back();
        std::cout<<"SAVE array to memory "<<rest<< std::endl;
        if (isDigits(rest))
        {
            saveConstTableToMemory(b, r, freeRegister, std::stoull(rest));
        }
        else 
        {
            saveVarTableToMemory(b, r, freeRegister, rest);
        }
    }
}

void RegisterBlock::saveVarTableToMemory(Block& b, Register& r, Register& freeRegister, std::string name)
{

    std::string array = r.variableName.substr(0, r.variableName.find("("));
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);

    loadVariableFromMemory(b, name, freeRegister, r);
    b.lines.push_back("\t#SAVING var array " + r.variableName + " from " + r.name + " using " + freeRegister.name);    
    b.lines.push_back({"COPY", "A", freeRegister.name});

    auto linesToAdd = generateNumber(shift, memoryCell, freeRegister);
    b.insert(linesToAdd);

    b.lines.push_back({"STORE", r.name});    
    b.lines.push_back("\t#END OF SAVING " + r.variableName + " from " + r.name + " using " + freeRegister.name);
}

 void RegisterBlock::saveVariableToMemory(Block& b, Register& r, Register& freeRegister)
 {
    unsigned long long memoryCell = _symbolTable->getMemoryCell(r.variableName);
    
    b.lines.push_back("\t#SAVING var " + r.variableName + " from " + r.name + " using " + freeRegister.name);
    auto lines = NumberGenerator::generateConstFrom(memoryCell, {{"A", 0}});
    lines.push_back({"STORE", r.name});
    b.insert(lines);
    b.lines.push_back("\t#END OF SAVING " + r.variableName + " from " + r.name + " using " + freeRegister.name);
 }

void RegisterBlock::saveConstTableToMemory(Block& b, Register& r, Register& freeRegister, unsigned long long diff)
{
    std::string array = r.variableName.substr(0, r.variableName.find("("));
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);
    memoryCell = memoryCell + diff - shift;

    b.lines.push_back("\t#SAVING const table " + r.variableName + " from " + r.name + " using " + freeRegister.name);
    auto lines = NumberGenerator::generateConstFrom(memoryCell, {{"A", 0}});
    lines.push_back({"STORE", r.name});
    b.insert(lines);
    b.lines.push_back("\t#END OF SAVING " + r.variableName + " from " + r.name + " using " + freeRegister.name);   
}

/***************************************/
// LOADING TO MEMORY SECTION
/***************************************/

void RegisterBlock::loadFromMemory(Block& b, std::string name, Register& r, Register& freeRegister)
{
    if (_symbolTable->isItVariable(name))
    {
        b.lines.push_back("#THIS IS A VARIABLE " + name);
        loadVariableFromMemory(b, name, r, freeRegister);
    }
    else if (!_symbolTable->isConst(name))
    {
        std::string rest = name.substr(name.find("(") + 1, std::string::npos);
        rest.pop_back();
        std::cout<<"LOAD array to memory "<<rest<< std::endl;
        if (isDigits(rest))
        {
            loadConstTableFromMemory(b, name, r, freeRegister, std::stoull(rest));
        }
        else
        {
            loadVarTableFromMemory(b, name, r, freeRegister, rest);
        }
    }   
}

void RegisterBlock::loadVarTableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister, std::string addresVariableName)
{
    std::string array = name.substr(0, name.find("("));
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);
    std::cout<<array<<" "<<shift<<" "<<memoryCell<<std::endl;

    loadVariableFromMemory(b, addresVariableName, freeRegister, r);
    b.lines.push_back("\t#LOADING var array " + r.variableName + " from " + r.name + " using " + freeRegister.name);    
    b.lines.push_back({"COPY", "A", freeRegister.name});


    auto linesToAdd = generateNumber(shift, memoryCell, freeRegister);
    b.insert(linesToAdd);

    b.lines.push_back({"LOAD", r.name});    
    b.lines.push_back("\t#END OF LOADING " + r.variableName + " from " + r.name + " using " + freeRegister.name);
}

void RegisterBlock::loadVariableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister)
{
    unsigned long long memoryCell = _symbolTable->getMemoryCell(name);

    b.lines.push_back("\t#LOADING var " + name + " to " + r.name + " using " + freeRegister.name);

    bool found = false;
    for (auto& regs : _registers)
    {
        if (regs.state == RegisterState::VARIABLE && regs.variableName == name)
        {
            found = true;
            b.lines.push_back({"COPY", r.name, regs.name});
        }
    }

    if (!found)
    {
        auto lines = NumberGenerator::generateConstFrom(memoryCell, {{"A", 0}});
        lines.push_back({"LOAD", r.name});
        b.insert(lines);
    }

    b.lines.push_back("\t#END OF LOADING " + name + " to " + r.name + " using " + freeRegister.name);
}


void RegisterBlock::loadConstTableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister, unsigned long long diff)
{
    std::string array = name.substr(0, name.find("("));
    std::cout<<array<<std::endl;        
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);
    memoryCell = memoryCell + diff - shift;

    b.lines.push_back("\t#LOADING const table " + name + " to " + r.name + " using " + freeRegister.name);
    auto lines = NumberGenerator::generateConstFrom(memoryCell, {{"A", 0}});
    lines.push_back({"LOAD", r.name});
    b.insert(lines);
    b.lines.push_back("\t#END OF LOADING " + name + " to " + r.name + " using " + freeRegister.name);
}


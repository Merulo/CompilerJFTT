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

std::vector<Line> RegisterBlock::performTableMemoryOperation(std::string operation, std::string varName, Register& r)
{
    std::string array = varName.substr(0, varName.find("("));

    unsigned long long memoryCell = _symbolTable->getMemoryCell(array);
    std::string rest = varName.substr(varName.find("(") + 1, std::string::npos);
    rest.pop_back();
    unsigned long long shift = _symbolTable->getTableShift(array);

    if (isDigits(rest))
    {
        memoryCell = memoryCell + std::stoull(rest) - shift;
        return performMemoryOperation(operation, r, memoryCell);
    }
    else
    {
        std::vector<Line> lines;
        Block b;
        Register& registerForShift = getUniqueRegisterForVariable(rest, b, {r}); 
        if (registerForShift.variableName != rest)
        {
            lines = performMemoryOperation("STORE", registerForShift, _symbolTable->getMemoryCell(registerForShift.variableName));
            auto ls = performMemoryOperation("LOAD", registerForShift, _symbolTable->getMemoryCell(rest));
            lines.insert(lines.end(), ls.begin(), ls.end());
            registerForShift.variableName = rest;
            registerForShift.state = RegisterState::VARIABLE;
        }
        _addressRegister.state = RegisterState::UNKNOWN;
        lines.push_back({"COPY", "A", registerForShift.name});

        generateNumber(lines, shift, memoryCell);

        lines.push_back({operation, r.name});
        return lines;
    }
}

void RegisterBlock::generateNumber(std::vector<Line>& lines, unsigned long long& firstNumber, unsigned long long second)
{
    while(firstNumber > second)
    {
        lines.push_back({"DEC", "A"});
        firstNumber--;
    }

    while( firstNumber < second)
    {
        lines.push_back({"INC", "A"});
        firstNumber++;
    }
}

std::vector<Line> RegisterBlock::prepareAddressRegister(unsigned long long memoryCell)
{
    std::vector<Line> lines;
    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        lines.push_back({"SUB", "A", "A"});
        _addressRegister.constValue = 0;
        _addressRegister.state = RegisterState::CONST;
    }

    generateNumber(lines, _addressRegister.constValue, memoryCell);

    return lines;    
}

std::vector<Line> RegisterBlock::performMemoryOperation(std::string operation, Register& r, unsigned long long memoryCell)
{
    std::vector<Line> lines = prepareAddressRegister(memoryCell);

    lines.push_back({operation , r.name});
    return lines;
}

bool checkRegisterHasState(RegisterState state, Register& r, std::vector<std::reference_wrapper<Register>> usedRegisters)
{
    if (r.state == state)
    {
        if(std::find_if(usedRegisters.begin(), usedRegisters.end(), [r](auto i)
            {
                return i.get() == r;
            }) == usedRegisters.end())
        {
            return true;
        }
    }
    return false;
}

bool checkRegisterHasVariable(std::string name, Register& r, std::vector<std::reference_wrapper<Register>> usedRegisters)
{
    if  (r.state == RegisterState::CONSTVARIABLE 
        || r.state == RegisterState::VARIABLE 
        || r.state == RegisterState::TABLE 
        || r.state == RegisterState::CONST)
    {
        if (r.variableName == name && std::find_if(usedRegisters.begin(), usedRegisters.end(), [r](auto i)
            {
                return i.get() == r;
            }) == usedRegisters.end())
        {
            return true;
        }
    }
    return false;
}

Register& RegisterBlock::getUniqueRegisterForVariable(std::string name, Block& b, std::vector<std::reference_wrapper<Register>> usedRegisters)
{
    if (!b.lines.empty())
    {
        saveTablesIfNeeded(name, b);
    }

    for(auto& r : _registers)
    {
        if (checkRegisterHasVariable(name, r, usedRegisters))
        {
            return r;
        }
    }

    for(auto&r : _registers)
    {
        if (checkRegisterHasState(RegisterState::UNKNOWN, r, usedRegisters))
        {
            return r;
        }
    }

    for(auto& r : _registers)
    {
        if (checkRegisterHasState(RegisterState::CONST, r, usedRegisters))
        {
            return r;
        }
    }

    Register& r = _registers[_currentRegister];
    _currentRegister++;
    if (_currentRegister == _registers.size())
    {
        _currentRegister = 0;
    }
    if (std::find_if(usedRegisters.begin(), usedRegisters.end(), [r](auto i)
            {
                return i.get() == r;
            }) == usedRegisters.end())
    {
        return r;
    }
    return getUniqueRegisterForVariable(name, b, usedRegisters); 
}

void RegisterBlock::saveTablesIfNeeded(std::string name, Block& b)
{
    for(auto& r : _registers)
    {
        if(_symbolTable->isItTable(name))
        {
            if (!_symbolTable->isItTable(r.variableName))
            {
                continue;
            }
            std::string currentTable = name.substr(0, name.find("("));
            std::string registerTable = r.variableName.substr(0, r.variableName.find("("));
            if (currentTable == registerTable)
            {
                saveTableToMemory(b, r);
                return;
            }
        }
        else
        {
            if (r.variableName.find("(") == std::string::npos)
            {
                continue;
            }
            std::string rest = r.variableName.substr(r.variableName.find("(") + 1, std::string::npos);
            rest.pop_back();
            if (name == rest)
            {
                saveTableToMemory(b, r);
                return;                   
            }
        }
    }
}

void RegisterBlock::saveTableToMemory(Block& b, Register& r)
{
    b.lines.push_back({"#saving " + r.variableName + " to memory"});
    auto lines = performTableMemoryOperation("STORE", r.variableName, r);
    b.lines.insert(b.lines.end(), lines.begin(), lines.end());
    r.variableName = "";
    r.state = RegisterState::UNKNOWN;
    b.lines.push_back({"#end of saving to memory"});
}

void RegisterBlock::print()
{
    std::cerr<<_addressRegister<<std::endl;
    for(auto& r : _registers)
    {
        std::cerr<<r<<std::endl;
    }
}

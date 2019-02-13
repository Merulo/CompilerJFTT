#include "RegisterBlock.hpp"

#include "IRs/FourthIR.hpp"

RegisterBlock::RegisterBlock(std::shared_ptr<SymbolTable> symbolTable)
{
    _symbolTable = symbolTable;
    _currentRegister = 0;
}

std::vector<Register> RegisterBlock::getRegisters()
{
    return _registers;
}

RegisterBlock::RegisterBlock(const RegisterBlock &rb){
    for(auto r : rb._registers)
    {
        Register copy = r;
        _registers.push_back(copy);
    }
    _addressRegister = rb._addressRegister;
    _currentRegister = rb._currentRegister;
    _symbolTable = rb._symbolTable;
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

void RegisterBlock::exitBlock(Block& b, RegisterBlock& other, IRBase& irBase)
{
    //saves to memory
    for(size_t i = 0; i < _registers.size(); i++)
    {
        if (_registers[i].variableName == other._registers[i].variableName)
        {
            continue;
        }
        if (other._registers[i].state == RegisterState::UNKNOWN)
        {
            FourthIR& fourthIR = static_cast<FourthIR&>(irBase);

            bool result = fourthIR.isThisVariableUsed(_registers[i].variableName, b);
            if (result)
            {
                continue;
            }
        }
        saveToMemory(b, _registers[i], _addressRegister);
    }

    //loads tables to registers
    for(size_t i = 0; i < _registers.size(); i++)
    {
        if (_registers[i].variableName == other._registers[i].variableName)
        {
            continue;
        } 
        if (other._registers[i].state != RegisterState::TABLE)
        {
            continue;
        }       
        _registers[i].variableName = "";
        _registers[i].state = RegisterState::UNKNOWN;
        loadFromMemory(b, other._registers[i].variableName, _registers[i], _addressRegister);
    }

    //loads variables to registers
    for(size_t i = 0; i < _registers.size(); i++)
    {
        if (_registers[i].variableName == other._registers[i].variableName)
        {
            continue;
        }
        if (other._registers[i].state == RegisterState::TABLE)
        {
            continue;
        }           
        _registers[i].variableName = "";
        _registers[i].state = RegisterState::UNKNOWN;
        loadFromMemory(b, other._registers[i].variableName, _registers[i], _addressRegister);
    }

    //sets A register
    if (other._addressRegister.state != RegisterState::UNKNOWN)
    {
        if (_addressRegister.state == RegisterState::UNKNOWN)
        {
            b.lines.push_back({"SUB", "A", "A"});
            _addressRegister.state = RegisterState::CONST;
            _addressRegister.constValue = 0;
        }
        auto lines = generateNumberFrom(_addressRegister.constValue, other._addressRegister.constValue, _addressRegister);
        b.insert(lines);
    }
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
                }) != usedRegisters.end())
        {      
            continue;
        }  
        if (reg.variableName == name)
        {
            saveToMemory(b, reg, _addressRegister);
            return reg;
        }
    }

    auto& reg = _registers[_currentRegister];
    _currentRegister++;
    if (_currentRegister == _registers.size())
    {
        _currentRegister = 0;
    }

    if (std::find_if(usedRegisters.begin(), usedRegisters.end(), [reg](auto i)
            {
                return i.get() == reg;
            }) == usedRegisters.end())
    {
        saveToMemory(b, reg, _addressRegister);
        loadFromMemory(b, name, reg, _addressRegister);
        return reg;
    }    

    return getRegistersForOperation(name, b, usedRegisters);     
}

Register& RegisterBlock::getRegister(std::string name, 
    Block& b, std::vector<std::reference_wrapper<Register>> usedRegisters, 
    bool changeValue, bool load)
{
    if (changeValue)
    {
        storeSameTable(name , b);
    }
    for(auto& reg : _registers)
    {
        if (std::find_if(usedRegisters.begin(), usedRegisters.end(), [reg](auto i)
                {
                    return i.get() == reg;
                }) != usedRegisters.end())
        {      
            continue;
        }         
        if (reg.variableName == name)
        {
            return reg;
        }
    }
    for(auto& reg : _registers)
    {
        if (reg.state == RegisterState::UNKNOWN)
        {
            if (load)
            {
                loadFromMemory(b, name, reg, _addressRegister);
            }
            return reg;
        }
    }

    auto & reg = _registers[_currentRegister];
    _currentRegister++;
    if (_currentRegister == _registers.size())
    {
        _currentRegister = 0;
    }

    if (reg.state == RegisterState::CONST)
    {
        return getRegister(name, b, usedRegisters, changeValue, load);
    }

    if (std::find_if(usedRegisters.begin(), usedRegisters.end(), [reg](auto i)
            {
                return i.get() == reg;
            }) == usedRegisters.end())
    {
        saveToMemory(b, reg, _addressRegister);
        if (load)
        {
            loadFromMemory(b, name, reg, _addressRegister);
        }        
        return reg;
    }    

    return getRegister(name, b, usedRegisters, changeValue, load); 
}

void RegisterBlock::storeSameTable(std::string name, Block& b)
{
    std::string currentTable = name.substr(0, name.find("("));
    for(auto& reg : _registers)
    {
        if (reg.state != RegisterState::TABLE)
        {
            continue;
        }
        if (name == reg.variableName)
        {
            continue;
        }
        std::string registerTable = reg.variableName.substr(0, reg.variableName.find("("));
        if (registerTable == currentTable)
        {
            saveToMemory(b, reg, _addressRegister);
            reg.state = RegisterState::UNKNOWN;
            reg.variableName = "";
            continue;
        }
        std::string rest = reg.variableName.substr(reg.variableName.find("(") + 1, std::string::npos);
        rest.pop_back();
        if (name == rest)
        {
            saveToMemory(b, reg, _addressRegister);
            reg.state = RegisterState::UNKNOWN;
            reg.variableName = "";
            continue;            
        }
    }
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
    return {{}};
}

std::vector<Line> RegisterBlock::generateNumberFrom(unsigned long long& firstNumber, unsigned long long second, Register& usedRegister)
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
            firstNumber = second;
            return NumberGenerator::generateConstFrom(second, {{"A", 0}});     
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
            firstNumber = second;
            return NumberGenerator::generateConstFrom(second, {{"A", 0}});   
        }
    }
    return {{}};
}


std::vector<Line> RegisterBlock::generateNumberShift(unsigned long long value, Register& freeRegister, std::string operation)
{
    auto linesToAdd = NumberGenerator::generateConstFrom(value, {{freeRegister.name, 0}});
    linesToAdd.push_back({operation, "A", freeRegister.name});
    return linesToAdd;
}

/***************************************/
// SAVING TO MEMORY SECTION
/***************************************/

void RegisterBlock::saveToMemory(Block& b, Register& r, Register& freeRegister)
{
    if ((r.state == RegisterState::VARIABLE || r.state == RegisterState::CONSTVARIABLE) && !r.variableName.empty())
    {
        if (r.variableName == "IF_CONTROL_VARIABLE")
        {
            return;
        }
        if (r.needToSafe)
        {
            saveVariableToMemory(b, r, freeRegister);
        }
    }
    else if (r.state == RegisterState::TABLE)
    {
        std::string rest = r.variableName.substr(r.variableName.find("(") + 1, std::string::npos);
        rest.pop_back();
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

    Register& regToSub = getRegister("", b, {r, freeRegister}, false, true);

    loadVariableFromMemory(b, name, freeRegister, r);
    regToSub.variableName = "";
    regToSub.state = RegisterState::UNKNOWN;

    auto linesToAdd = generateNumber(shift, memoryCell, regToSub);
    _addressRegister.state = RegisterState::UNKNOWN;    
    b.insert(linesToAdd);

    b.lines.push_back({"STORE", r.name});    
}

 void RegisterBlock::saveVariableToMemory(Block& b, Register& r, Register& freeRegister)
 {
    unsigned long long memoryCell = _symbolTable->getMemoryCell(r.variableName);
    
    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        b.lines.push_back({"SUB", "A", "A"});
        _addressRegister.state = RegisterState::CONST;
        _addressRegister.constValue = 0;
    }
    auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

    lines.push_back({"STORE", r.name});
    b.insert(lines);
 }

void RegisterBlock::saveConstTableToMemory(Block& b, Register& r, Register& freeRegister, unsigned long long diff)
{
    std::string array = r.variableName.substr(0, r.variableName.find("("));
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);
    memoryCell = memoryCell + diff - shift;

    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        b.lines.push_back({"SUB", "A", "A"});
        _addressRegister.state = RegisterState::CONST;
        _addressRegister.constValue = 0;
    }
    auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

    lines.push_back({"STORE", r.name});
    b.insert(lines);
}

/***************************************/
// LOADING TO MEMORY SECTION
/***************************************/

void RegisterBlock::loadFromMemory(Block& b, std::string name, Register& r, Register& freeRegister)
{
    r.needToSafe = false;
    if (_symbolTable->isItVariable(name))
    {
        if (name == "IF_CONTROL_VARIABLE")
        {
            return;
        }        
        loadVariableFromMemory(b, name, r, freeRegister);
    }
    else if (_symbolTable->isItTable(name))
    {
        std::string rest = name.substr(name.find("(") + 1, std::string::npos);
        rest.pop_back();
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

    Register& regToSub = getRegister("", b, {r, freeRegister}, false, true);
    loadVariableFromMemory(b, addresVariableName, freeRegister, r);

    auto linesToAdd = generateNumber(shift, memoryCell, regToSub);
    regToSub.variableName = "";
    regToSub.state = RegisterState::UNKNOWN;

    _addressRegister.state = RegisterState::UNKNOWN;    
    b.insert(linesToAdd);

    b.lines.push_back({"LOAD", r.name});    
}

void RegisterBlock::loadVariableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister)
{
    unsigned long long memoryCell = _symbolTable->getMemoryCell(name);

    bool found = false;
    for (auto& regs : _registers)
    {
        if ((regs.state == RegisterState::VARIABLE || regs.state == RegisterState::CONSTVARIABLE) && regs.variableName == name)
        {
            found = true;
            b.lines.push_back({"COPY", r.name, regs.name});
        }
    }

    if (!found)
    {
        if (_addressRegister.state == RegisterState::UNKNOWN)
        {
            b.lines.push_back({"SUB", "A", "A"});
            _addressRegister.state = RegisterState::CONST;
            _addressRegister.constValue = 0;
        }
        auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

        lines.push_back({"LOAD", r.name});
        b.insert(lines);
    }
}


void RegisterBlock::loadConstTableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister, unsigned long long diff)
{
    std::string array = name.substr(0, name.find("("));
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);
    memoryCell = memoryCell + diff - shift;

    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        b.lines.push_back({"SUB", "A", "A"});
        _addressRegister.state = RegisterState::CONST;
        _addressRegister.constValue = 0;
    }
    auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

    lines.push_back({"LOAD", r.name});
    b.insert(lines);
}
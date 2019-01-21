#include "RegisterBlock.hpp"

RegisterBlock::RegisterBlock(std::shared_ptr<SymbolTable> symbolTable)
{
    _symbolTable = symbolTable;
    _currentRegister = 0;
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

void RegisterBlock::exitBlock(Block& b, RegisterBlock& other)
{
    for(size_t i = 0; i < _registers.size(); i++)
    {
        if (_registers[i].variableName == other._registers[i].variableName)
        {
            // std::cout<<"SKIPPING SAVING "<<_registers[i]<<std::endl;
            continue;
        }

        // std::cout<<"Saving "<<_registers[i]<<std::endl;
        saveToMemory(b, _registers[i], _addressRegister);
    }

    for(size_t i = 0; i < _registers.size(); i++)
    {
        if (_registers[i].variableName == other._registers[i].variableName)
        {
            // std::cout<<"SKIPPING LOADING "<<_registers[i]<<std::endl;
            continue;
        } 
        if (other._registers[i].state != RegisterState::TABLE)
        {
            continue;
        }       
        // std::cout<<"Loading "<<_registers[i]<<std::endl;
        _registers[i].variableName = "";
        _registers[i].state = RegisterState::UNKNOWN;
        loadFromMemory(b, other._registers[i].variableName, _registers[i], _addressRegister);
    }

    for(size_t i = 0; i < _registers.size(); i++)
    {
        if (_registers[i].variableName == other._registers[i].variableName)
        {
            // std::cout<<"SKIPPING LOADING "<<_registers[i]<<std::endl;
            continue;
        }
        if (other._registers[i].state == RegisterState::TABLE)
        {
            continue;
        }           
        // std::cout<<"Loading "<<_registers[i]<<std::endl;
        _registers[i].variableName = "";
        _registers[i].state = RegisterState::UNKNOWN;
        loadFromMemory(b, other._registers[i].variableName, _registers[i], _addressRegister);
    }

    if (other._addressRegister.state != RegisterState::UNKNOWN)
    {
        // std::cout<<"change "<<_addressRegister<<std::endl;
        // std::cout<<"to "<<other._addressRegister<<std::endl;
        if (_addressRegister.state == RegisterState::UNKNOWN)
        {
            b.lines.push_back({"SUB", "A", "A"});
            _addressRegister.state = RegisterState::CONST;
            _addressRegister.constValue = 0;
        }

        b.lines.push_back({"#START OF SETTING MEMORY REGISTER"});
        auto lines = generateNumberFrom(_addressRegister.constValue, other._addressRegister.constValue, _addressRegister);
        b.insert(lines);
        b.lines.push_back({"#END OF SETTING MEMORY REGISTER"});
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
    b.lines.push_back({"#SEARCHING FOR " + name});
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
    return {{"#No need to generate number"}};
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
    return {{"#No need to generate number"}};
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
    if (r.state == RegisterState::VARIABLE && !r.variableName.empty())
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

    Register& regToSub = getRegister("", b, {r, freeRegister}, false, true);
    b.lines.push_back({"#GOT " + regToSub.name+" with "+regToSub.variableName+" "});
    b.lines.push_back({"#GOT " + r.name+" with "+r.variableName+" "});
    b.lines.push_back({"#GOT " + freeRegister.name+" with "+freeRegister.variableName+" "});

    loadVariableFromMemory(b, name, freeRegister, r);
    b.lines.push_back("\t#SAVING var array " + r.variableName + " from " + r.name + " using " + freeRegister.name);    
    regToSub.variableName = "";
    regToSub.state = RegisterState::UNKNOWN;

    auto linesToAdd = generateNumber(shift, memoryCell, regToSub);
    _addressRegister.state = RegisterState::UNKNOWN;    
    b.insert(linesToAdd);

    b.lines.push_back({"STORE", r.name});    
    b.lines.push_back("\t#END OF SAVING " + r.variableName + " from " + r.name + " using " + freeRegister.name);
}

 void RegisterBlock::saveVariableToMemory(Block& b, Register& r, Register& freeRegister)
 {
    unsigned long long memoryCell = _symbolTable->getMemoryCell(r.variableName);
    
    b.lines.push_back("\t#SAVING var " + r.variableName + " from " + r.name + " using " + freeRegister.name);

    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        b.lines.push_back({"SUB", "A", "A"});
        _addressRegister.state = RegisterState::CONST;
        _addressRegister.constValue = 0;
    }
    auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

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
    
    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        b.lines.push_back({"SUB", "A", "A"});
        _addressRegister.state = RegisterState::CONST;
        _addressRegister.constValue = 0;
    }
    auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

    lines.push_back({"STORE", r.name});
    b.insert(lines);
    b.lines.push_back("\t#END OF SAVING " + r.variableName + " from " + r.name + " using " + freeRegister.name);   
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
        b.lines.push_back("\t#THIS IS A VARIABLE " + name);
        loadVariableFromMemory(b, name, r, freeRegister);
    }
    else if (_symbolTable->isItTable(name))
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

    Register& regToSub = getRegister("", b, {r, freeRegister}, false, true);
    loadVariableFromMemory(b, addresVariableName, freeRegister, r);
    b.lines.push_back("\t#LOADING var array " + name + " to " + r.name + " using " + freeRegister.name);  

    auto linesToAdd = generateNumber(shift, memoryCell, regToSub);
    regToSub.variableName = "";
    regToSub.state = RegisterState::UNKNOWN;

    _addressRegister.state = RegisterState::UNKNOWN;    
    b.insert(linesToAdd);

    // b.lines.push_back({"PUT", "A"});
    b.lines.push_back({"LOAD", r.name});    
    b.lines.push_back("\t#END OF LOADING " + r.variableName + " from " + r.name + " using " + freeRegister.name);
}

void RegisterBlock::loadVariableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister)
{
    unsigned long long memoryCell = _symbolTable->getMemoryCell(name);

    b.lines.push_back("\t#LOADING var " + name + " to " + r.name);

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

    b.lines.push_back("\t#END OF LOADING " + name + " to " + r.name);
}


void RegisterBlock::loadConstTableFromMemory(Block& b, std::string name, Register& r, Register& freeRegister, unsigned long long diff)
{
    std::string array = name.substr(0, name.find("("));
    std::cout<<array<<std::endl;        
    unsigned long memoryCell = _symbolTable->getMemoryCell(array);
    unsigned long long shift = _symbolTable->getTableShift(array);
    memoryCell = memoryCell + diff - shift;

    b.lines.push_back("\t#LOADING const table " + name + " to " + r.name + " using " + freeRegister.name);
    
    if (_addressRegister.state == RegisterState::UNKNOWN)
    {
        b.lines.push_back({"SUB", "A", "A"});
        _addressRegister.state = RegisterState::CONST;
        _addressRegister.constValue = 0;
    }
    auto lines = generateNumberFrom(_addressRegister.constValue, memoryCell, freeRegister);

    lines.push_back({"LOAD", r.name});
    b.insert(lines);
    b.lines.push_back("\t#END OF LOADING " + name + " to " + r.name + " using " + freeRegister.name);
}
#include "RegisterAllocator.hpp"

#include "Calculators/NumberGenerator.hpp"

RegisterAllocator::RegisterAllocator(SymbolTable& symbolTable)
    : _symbolTable(symbolTable)
{
    //should be i < 8
    for(int i = 1; i < 4; i++)
    {
        Register r;
        std::string str;
        str.push_back(char (int ('A') + i));
        r.registerName = str;
        _registers.push_back(r);
    }
}

RegisterAllocator::~RegisterAllocator()
{
    for(auto& s : _lines)
    {
        _outputFile<<s<<std::endl;
    }
}

void RegisterAllocator::print()
{
    std::cerr<<"**************************"<<std::endl;
    std::cerr<<"Registers:"<<std::endl;
    std::cerr<<_addressRegister.registerName<<" "<<_addressRegister.value<<std::endl;
    for(auto& r : _registers)
    {
        std::cerr<<r.registerName<<" "<<r.value<<" "<<r.variableName<<std::endl;
    }
}

void RegisterAllocator::compile(const std::string& fileName, std::vector<Line> finalIR)
{
    _outputFile.open(fileName);
    std::cerr<<"compiling!"<<std::endl;

    for (auto& l : finalIR)
    {
        // std::cerr<<l.operation<<" "<<l.one<<" "<<l.two<<std::endl;
        if (l.operation == "READ")
        {
            handleRead(l.one);
        }
        if (l.operation == "WRITE")
        {
            handleWrite(l.one);
        }
        if (l.operation == "HALT")
        {
            _lines.push_back("HALT");
        }
        if (l.operation == "CONST")
        {
            handleConst(l.one, std::stoull(l.two));
        }
        if (l.operation == "COPY")
        {
            handleCopy(l);
        }
        if (l.operation == "ADD" || l.operation == "SUB")
        {
            handleSimpleMath(l);
        }
        if (l.operation == "JUMP" || l.operation == "JZERO")
        {
            handleJump(l);
        }
        if (!l.thisLabel.empty())
        {
            handleLabel(l);
        }
    }
}

/*
PRIVATE
*/

void RegisterAllocator::handleRead(std::string& variableName)
{
    
    Register r = getRegisterForVariable(variableName).first;
    _lines.push_back("GET " + r.registerName);
    _registers.push_back(r);
}

void RegisterAllocator::handleWrite(std::string& variableName)
{
    auto pair = getRegisterForVariable(variableName);
    loadIfNeeded(pair.first, pair.second);
    _lines.push_back("PUT " + pair.first.registerName);
    _registers.push_back(pair.first);
} 

void RegisterAllocator::handleConst(std::string variableName, ull value)
{
    Register r = getRegisterForVariable(variableName).first;
    auto resultVec = NumberGenerator::generateConstFrom(value, {{r.registerName, r.value}});

    _lines.insert(_lines.end(), resultVec.begin(), resultVec.end());
    r.value = value;
    _registers.push_back(r);
}

void RegisterAllocator::handleCopy(Line line)
{
    auto firstRegister = getRegisterForVariable(line.one);
    auto secondRegister = getRegisterForVariable(line.two);
    loadIfNeeded(firstRegister.first, firstRegister.second);
    loadIfNeeded(secondRegister.first, secondRegister.second);    
    _lines.push_back("COPY " + firstRegister.first.registerName + " " + secondRegister.first.registerName);
    _registers.push_back(firstRegister.first);
    _registers.push_back(secondRegister.first);
}

void RegisterAllocator::handleSimpleMath(Line line)
{
    auto firstRegister = getRegisterForVariable(line.one);
    auto secondRegister = getRegisterForVariable(line.two);
    //todo: decide based which one will be cheaper
    loadIfNeeded(firstRegister.first, firstRegister.second);
    loadIfNeeded(secondRegister.first, secondRegister.second);
    std::cerr<<"TEST "<<firstRegister.first<<" " <<secondRegister.first<<std::endl;
    _lines.push_back(line.operation + " " + firstRegister.first.registerName + " " + secondRegister.first.registerName);
    _registers.push_back(firstRegister.first);
    _registers.push_back(secondRegister.first);    
}

void RegisterAllocator::handleJump(Line line)
{
    auto r = getRegisterForVariable(line.one);
    _lines.push_back(line.operation + " " + r.first.registerName + " " + line.targetLabel);
    _registers.push_back(r.first);
}

void RegisterAllocator::handleLabel(Line line)
{
    std::string searched = line.thisLabel;
    std::cerr<<"searching for "<<searched<<std::endl;
    size_t currentInstruction = _lines.size();
    for(auto& l : _lines)
    {
        if (l.find(searched) != std::string::npos)
        {
            l.replace(l.find(searched), searched.length(), std::to_string(currentInstruction));
            return;
        }
    }
}

//todo upgrade this so that less calls to saving [Curent state: ROUND ROBIN]
std::pair<Register, bool> RegisterAllocator::getRegisterForVariable(std::string name)
{
    // std::cerr<<"Looking for "<<name<<std::endl;
    for(auto r : _registers)
    {
        // std::cerr<<"\tTesting "<<r.variableName<<std::endl;
        if(r.variableName == name)
        {
            Register result = r;
            _registers.erase(std::remove(_registers.begin(), _registers.end(), r), _registers.end());
            result.variableName = name;
            return {result, true};
        }
    }

    if(_registers.size() == 0)
    {
        std::cerr<<"REGISTERS WERE NOT RETURNED TO POOL"<<std::endl;
    }

    //todo: first choose a register with variable_x or temporary

    Register r = _registers.front();
    _registers.pop_front();
    // _registers.push_back(r);
    if (!r.variableName.empty() && !_symbolTable.isConst(r.variableName))
    {
        prepareAddressRegister(r);
        _lines.push_back("STORE "+r.registerName);
    }
    r.variableName = name;
    return {r, false};
}

void RegisterAllocator::loadIfNeeded(Register& r, bool value)
{
    if(!value && !_symbolTable.isConst(r.variableName))
    {
        prepareAddressRegister(r);
        r.value = 0;
        _lines.push_back("LOAD " + r.registerName);

    }
}

void RegisterAllocator::prepareAddressRegister(Register& r)
{
    ull memoryCell = _symbolTable.getMemoryCell(r.variableName);
    if (_addressRegister.registerName.empty())
    {
        _addressRegister.registerName = "A";
        _lines.push_back("SUB A A");
    }
    //todo check if any register has a close const number to this
    //todo check how to generate this constant fastest
    // std::cerr<<"seting address at "<<memoryCell<<" for "<<r.variableName<<std::endl;
    while(_addressRegister.value < memoryCell)
    {
        _lines.push_back("INC A");
        _addressRegister.value++;
    }
    while(_addressRegister.value > memoryCell)
    {
        _lines.push_back("DEC A");
        _addressRegister.value--;
    }
}



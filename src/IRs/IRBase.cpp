#include "IRBase.hpp"

IRBase::IRBase(std::string IRName) : _IRName(IRName)
{
}

void IRBase::print(cStrRef fileName)
{
    std::streambuf * buf;
    if (fileName.empty())
    {
        buf = std::cerr.rdbuf();
        std::ostream out(buf);
        writeToStream(out);
    }
    else
    {
        std::ofstream output(fileName);
        buf = output.rdbuf();
        std::ostream out(buf);
        writeToStream(out);
    }
}

void IRBase::writeToStream(std::ostream& stream)
{
    stream<<_IRName<<":"<<std::endl;
    for(auto& l : _lines)
    {
        if (!l.thisLabel.empty())
        {
            stream<<"#"<<l.thisLabel<<std::endl;
            continue;
        }
        stream<<l.operation<<": "<<l.one<<" "<<l.two<<" ";
        if (!l.targetLabel.empty())
        {
            stream<<"goto "<<l.targetLabel;
        }
        stream<<std::endl;
    }
}
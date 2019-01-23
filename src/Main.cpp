#include "External/Scanner.cpp"
#include "External/Parser.cpp"


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout<<"USAGE: ./main.exe input output"<<std::endl;
        std::cout<<"Not enough arguments\n";
        return 1;
    }
    d.FIR->setSymbolTable(d.ST);
    yyin = fopen(argv[1], "r");
    yyparse();

    bool emulator = true;

    for(int i = 0; i < argc; i++)
    {
        std::string str = argv[i];
        std::string output;
        if (i + 1 < argc)
        {
            output = argv[i+1];
        }
        if (str == "-em")
        {
            emulator = false;
        }        
        if (str == "-FIR")
        {
            d.FIR->print(output);
            return 0;
        }
        if (str == "-SIR")
        {
            d.convertToSIR();
            d.SIR->print(output);   
            return 0;
        }
        if (str == "-TIR")
        {
            d.convertToSIR();
            d.convertToTIR();
            d.TIR->print(output);
            return 0;
        }
        if (str == "-FOIR")
        {
            d.convertToSIR();
            d.convertToTIR();
            d.convertToFOIR(argv[2], emulator);
            d.FOIR->print(output);
            return 0;
        }
    }

    d.convertToSIR();
    d.convertToTIR();
    d.convertToFOIR(argv[2], emulator);
    d.converToFIIR();
    d.FIIR->print(argv[2]);

    return 0;
}
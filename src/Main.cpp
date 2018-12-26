#include "External/Scanner.cpp"
#include "External/Parser.cpp"


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout<<"Not enough arguments\n";
        return 1;
    }
    d.FIR->setSymbolTable(d.ST);
    yyin = fopen(argv[1], "r");
    yyparse();

    for(int i = 0; i < argc; i++)
    {
        std::string str = argv[i];
        if (str == "-FIR")
        {
            if (i + 1 < argc)
            {
                d.FIR->print(argv[i+1]);
            }
            else
            {
                d.FIR->print("");
            }
            return 0;
        }
        if (str == "-SIR")
        {
            d.convertToSIR();
            if (i + 1 < argc)
            {
                d.SIR->print(argv[i+1]);
            }
            else
            {
                d.SIR->print("");
            }            
            return 0;
        }
        if (str == "-TIR")
        {
            d.convertToSIR();
            d.convertToTIR();
            if (i + 1 < argc)
            {
                d.TIR->print(argv[i+1]);
            }
            else
            {
                d.TIR->print("");
            }            
            return 0;
        }              
    }
    // d.compile(argv[2]);
    // d.ST.print();


    return 0;
}
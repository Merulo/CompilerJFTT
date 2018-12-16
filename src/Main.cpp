#include "Scanner.cpp"
#include "Parser.cpp"


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout<<"Not enough arguments\n";
        return 1;
    }
    yyin = fopen(argv[1], "r");
    yyparse();

    for(int i = 0; i < argc; i++)
    {
        std::string str = argv[i];
        if (str == "-TAC")
        {
            d.TAC.print(argv[i+1]);
        }
    }

    // d.ST.print()
    d.TAC.print();


    return 0;
}
#include "Scanner.cpp"
#include "Parser.cpp"


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout<<"Niepoprawna liczba argumentów\n";
        return 1;
    }
    yyin = fopen(argv[1], "r");
    yyparse();


    return 0;
}
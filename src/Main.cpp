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

    driver.threeAddressCode.print();


    return 0;
}
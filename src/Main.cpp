#include "Parser.hpp"
#include "scanner.c"
#include "Logger.hpp"

int main()
{
    yyparse();
    return 0;
}
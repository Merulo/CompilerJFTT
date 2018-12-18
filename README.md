# Compiler

## TODO:
* Implement while loops
* Implement tables
* Implement for loop
* Register allocator
* Final IR
* Ensure values are initialized

##  Intermediate representations

### Three Address Code
| Operand | Arguments | Meaning |
| --- | --- | --- |
| READ | A | Read stdin to variable A |
| WRITE | A | Write A to stdout |
| CONST | A B | Set variable A value to number B |
| COPY | A B | Copy B value to A |
| ADD | A B | To A add B |
| SUB | A B | From A subtract B |
| MUL | A B | Multiply A by B |
| DIV | A B | Divide A by B |
| MOD | A B | Calculate A modulo B |
| JLS | A B L_X | If A is less than B jump to L_X |
| JMR | A B L_X | If A is more than B jump to L_X |
| JLE | A B L_X | If A is less or equal to B jump to L_X |
| JME | A B L_X | If A is more or equal to B jump to L_X |
| JNE | A B L_X | If A is NOT equal to B jump to L_X |
| JEQ | A B L_X | If A is equal to B jump to L_X |
| HALT | _none_ | End |

### Final IR

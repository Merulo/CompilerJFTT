# Compiler

## TODO:
* Implement while loops
* Implement tables and allocate memory for them
* Implement for loop
* Register allocator
* Final IR
* Ensure values are initialized

##  Intermediate representations

### First IR
First IR is created by blocks in format:
```cpp
    std::string blockName;
    std::vector<Line> lines;
    std::string blockIfTrue;
    std::string blockIfFalse;
    std::string blockJump;
```
Transition between blocks:
* If the last line is a condition then calculate result and follow blockIfTrue or blockIfFalse
* If there is no condition then follow blockJump
* If there is no blockJump the go to next block in storage order

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
| JLS | A B | If A is less than B do block jump |
| JMR | A B | If A is more than B do block jump |
| JLE | A B | If A is less or equal to B do block jump |
| JME | A B | If A is more or equal to B do block jump |
| JNE | A B | If A is NOT equal to B do block jump |
| JEQ | A B | If A is equal to B do block jump |
| HALT | _none_ | End |
### Final IR

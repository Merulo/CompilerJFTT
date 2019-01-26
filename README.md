# Compiler
Created by as part of JFTT (J&#281;zyki Formalne i Teoria Translacji). Compiles programs written in simple imperative language for Register Machine.

## General Information
Compiler is written in C++ with small parts of C. Tests are run using python.

Tools and their version used in compiling:
* Linux!
* make 4.1
* gcc 7.3
* bison 3.0.4
* flex 2.6.4

To run tests:
* python 3.6.7

## Project structure
### registerMachine/
Contains code for Register Machine and couple of assembler programs, including fast mult, mul and div.
### src/
#### Calculators/ 
General directory for static classes generating code for complex math operations and sub optimal const generation.
#### DataTypes/
Simple directory with commonly used structs.
#### External/
The bison and flex code is in this directory. Also contains static class for running simple python emulator.
#### IRs/
All intermediate representations are written in this directory. More about them below.
#### Register/
Class representation of register and all of them. Registers know their state and value (if it's const). RegisterBlock handles register allocation and memory operations. 
### standalone/
Couple of small programs written in C++ used for debugging programs or concept testing of ideas.
### tests/
Directory containing tests. They can be run by entering this directory and typing:
```
python testRunner.py
```
Directories 01-03 contain IR specific tests so they are probably useless outside of this projects. Director 04 contains tests for basic scenarios. Directory 05 contains tests necessary for passing the course. Directories 06. and 07. are extra tests for testing implemented optimizations and general stuff.

##  Intermediate representations
### BaseIR
The base of IR is represented in Block form:
```
#blockName
    <Instructions>
#jump to nextBlockName
#end of blockName
```
If a block ends with a condition-like instruction:
```
#blockName2
    <Instructions>
#ifTrue blockNameIfConditionTrue
#ifFalse blockNameIfConditionFalse
#end of blockName2
```

### FirstIR
FirstIR is a direct creation of bison parser. Due to the way this parsing works the jumps between blocks are not always explicit. If there is no jump instruction at the end of block it should jump to the next stored (not named!) block.
Sometimes block with no instruction lines can appear. This is to help navigate jumps.

#### General opcodes
| Operand   | Arguments | Meaning                                       |
| ---       | ---       | ---                                           |
| READ      | A         | Read stdin to variable A                      |
| WRITE     | A         | Write A to stdout                             |
| CONST     | A B       | Set variable A value to number B              |
| COPY      | A B       | Copy B value to A                             |
| INC       | A         | Increase value of A by 1                      |
| DEC       | A         | Decrease value of A by 1                      |
| ADD       | A B       | To A add B                                    |
| SUB       | A B       | From A subtract B                             |
| MUL       | A B       | Multiply A by B                               |
| DIV       | A B       | Divide A by B                                 |
| MOD       | A B       | Calculate A modulo B                          |
| HALF      | A         | Half A                                        |
| HALT      | _none_    | End                                           |

#### Condition opcodes
| Operand   | Arguments | Meaning                                       |
| ---       | ---       | ---                                           |
| JUMP      | _none_    | Jump to next block                            |
| JZERO     | A         | If A is 0 then jump                           |
| JODD      | A         | If A is odd then do block jump                |
| JLS       | A B       | If A is less than B do block jump             |
| JMR       | A B       | If A is more than B do block jump             |
| JLE       | A B       | If A is less or equal to B do block jump      |
| JME       | A B       | If A is more or equal to B do block jump      |
| JNE       | A B       | If A is NOT equal to B do block jump          |
| JEQ       | A B       | If A is equal to B do block jump              |

### SecondIR
Second IR eliminates the problems created by FirstIR by removing implicit jumps and empty blocks.
Each block (except the last... there is nowhere to jump) ends with either conditional jumps or direct jump to other block.
This can be easily translated to a directed cycle graph.
Used optimizations:
* removing unused iterators
* removing add/sub for small consts
* fast mult/div by powers of 2
* fast mod by 2
* removing unused calculations
* replacing a/a by 1
* replacing a%a by 0

### ThirdIR
Optimizes futher and replaces illegal jump instruction with more legal ones. Main optimizations used here: removing copy X X and replacing jumps with zeros to simpler constructions.

### FourthIR + RegisterBlock
Replaces calculations done in virtual registers to one done in real registers.
Adds loading and storing from memory to registers.


### Fifth
Final IR replacing labels with proper jumps. Removs jumps when they are unnecessary.

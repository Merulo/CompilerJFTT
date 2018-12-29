# Compiler

## TODO:
* SecondIR
* ThirdIR
* FourthIR
* Register Allocator
* SSA <-> SecondIR

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
| HALT      | _none_    | End                                           |

#### Condition opcodes
| Operand   | Arguments | Meaning                                       |
| ---       | ---       | ---                                           |
| JZERO     | A         | If A is 0 then jump                           |
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
This is the part where optimization happens (to be done).

### ThirdIR
Should legalize all the code, meaning translating it directly into the assembler.
TODO: remove empty blocks

### FourthIR + RegisterAllocator
Should create executable code.
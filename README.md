# Brainfuck Fast Interpreter (NASM, x64)

## Brainfuck language

Brainfuck is an esoteric programming language created in 1993 by Urban Muller.

It consists of 8 commands:

| Character |Description                                                                                            |
|:---------:|:------------------------------------------------------------------------------------------------------|
|     >     |Increment the data pointer                                                                             |
|     <     |Decrement the data pointer                                                                             |
|     +     |Increment (increase by 1) the byte at the data pointer                                                 |
|     -     |Decrement (decrease by 1) the byte at the data pointer                                                 |
|     .     |Output the byte at the data pointer                                                                    |
|     ,     |Input one byte and storing the value in the byte at the data pointer                                   |
|     [     |If the byte at the data pointer is 0, then jump forward to the command after the matching ] command    |
|     ]     |If the byte at the data pointer is nonzero, then jump back to the command after the matching [ command |

A brainfuck program is a sequence of the commands, possibly interspersed with other characters (which are ignored). The commands are executed sequentially, with some exceptions: an instruction pointer begins at the first command, and each command it points to is executed, after which it normally moves forward to the next command. The program terminates when the instruction pointer moves past the last command.

More details on [wiki](https://en.wikipedia.org/wiki/Brainfuck)

## Basic idea

| Character |  C equivalent   | Assembler equivalent  |
|:---------:|:----------------|:----------------------|
|     >     |     ++ptr;      | inc RBX               |
|     <     |     --ptr;      | dec RBX               |
|     +     |     ++*ptr;     | inc byte [RBX]        |
|     -     |     --*ptr;     | dec byte [RBX]        |
|     .     | putchar(*ptr);  | sys_write             |
|     ,     |*ptr = getchar();| sys_read              |
|     [     |  while (*ptr) { | cmp byte [RBX], 0<br> jz  @to_close_bracket |
|     ]     |       }         | cmp byte [RBX], 0<br> jnz @to_open_bracket  |


## Mandelbrot (tiny)

```
AAAAAAAABBBBBBBBCCCCCCCCCCCCCCCCCCDDDDEFEEDDDCCCCCBBBBBBBBBBBBBBB
AAAAAAABBBBBBCCCCCCCCCCCCCCCCCDDDDDDEEFIKGGGDDDDDCCCCBBBBBBBBBBBB
AAAAAABBBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGHKPIGFEDDDDDCCCCCBBBBBBBBB
AAAAABBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGPVT  Q[HEEEEDDDCCCCCCBBBBBBB
AAAABBCCCCCCCCCCCCCCCCDDDDDDDEEFFFGGHK      HGFFEEEDDDCCCCCBBBBBB
AAABBCCCCCCCCCCCCCCCDDDDDEEEFGK MJJ NR    YS L HHGIJFDDCCCCCCBBBB
AAABCCCCCCCCCCCCCDDDEEEEEEFFFHI                    MGEDDCCCCCCBBB
AABCCCCCCCCCCCDDEEEEEEEEFFFGY Q                   MHGEEDCCCCCCCBB
AACCCCCCDDDDDEEFLHGGHMHGGGHIR                      QLHEDDCCCCCCCB
ABCCDDDDDDEEEEFGIKU    RLJJL                        IFEDDCCCCCCCB
ACDDDDDDEEEEEGGHOS        QR                        JFEDDDCCCCCCC
ADDDDDEFFFGGHKOPS                                   GEEDDDCCCCCCC
A                                                PJGFEEDDDCCCCCCC
ADDDDDEFFFGGHKOPS                                   GEEDDDCCCCCCC
ACDDDDDDEEEEEGGHOS        QR                        JFEDDDCCCCCCC
ABCCDDDDDDEEEEFGIKU    RLJJL                        IFEDDCCCCCCCB
AACCCCCCDDDDDEEFLHGGHMHGGGHIR                      QLHEDDCCCCCCCB
AABCCCCCCCCCCCDDEEEEEEEEFFFGY Q                   MHGEEDCCCCCCCBB
AAABCCCCCCCCCCCCCDDDEEEEEEFFFHI                    MGEDDCCCCCCBBB
AAABBCCCCCCCCCCCCCCCDDDDDEEEFGK MJJ NR    YS L HHGIJFDDCCCCCCBBBB
AAAABBCCCCCCCCCCCCCCCCDDDDDDDEEFFFGGHK      HGFFEEEDDDCCCCCBBBBBB
AAAAABBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGPVT  Q[HEEEEDDDCCCCCCBBBBBBB
AAAAAABBBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGHKPIGFEDDDDDCCCCCBBBBBBBBB
AAAAAAABBBBBBCCCCCCCCCCCCCCCCCDDDDDDEEFIKGGGDDDDDCCCCBBBBBBBBBBBB

real	0m0,166s
user	0m0,165s
sys	0m0,000s
```

## Fastest Brainfuck Interpreter

https://github.com/rdebath/Brainfuck/tree/master/tritium
# Fast Brainfuck Interpreter (NASM, x64)

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

More details on [wiki](https://en.wikipedia.org/wiki/Brainfuck).

## "Hello world" example

[hello.bf](https://github.com/dkozyr/brainfuck/blob/main/examples/hello.bf) script:

```
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.++
+.------.--------.>>+.>++.
```

It produces output:

```
Hello World!
```

## Interpreter - basic idea

Each brainfuck character (operand) has simple C and Assembler equivalent:

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

and on start `char* ptr` and register `RBX` are initialized to point to an array of 30'000 zero bytes. Interpreter reads brainfuck stript and processes operands one by one:
* get symbol
* find corresponding C/ASM equivalent
* execute the equivalent

Such interpreter easy to read ([interpreter.asm](https://github.com/dkozyr/brainfuck/blob/main/nasm_experiments/interpreter.asm)) but it works quite slow: 17.5 seconds for [tiny Mandelbrot](https://github.com/dkozyr/brainfuck/blob/main/examples/mandelbrot-tiny.bf) script:

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

real	0m17,495s
user	0m17,476s
sys	0m0,000s
```

## Interpreter with code generator

Brainfuck interpreter above takes too much CPU time on "operand interpreting" while "operand executing" in most cases only 1 CPU instruction (increment/decrement pointer, increment/decrement referenced value) and with several hierarchical cycles time wasting grows exponentially.

Let's optimize it! We interpret each symbol only once and generate code for it using "assembler equivalent". So the process is almost the same, interpreter generates instructions for each operand in some memory and doesn't execute each operand immediately, then it executes the whole generated code.

[interpreter_with_codegen.asm](https://github.com/dkozyr/brainfuck/blob/main/nasm_experiments/interpreter_with_codegen.asm) has a [low-level trick](https://github.com/dkozyr/brainfuck/blob/main/nasm_experiments/interpreter_with_codegen.asm#L86) to finish generated code correctly - the last generated operand is `ret` and we `push` address to proceed needed code after script executing:

```
    push    _close_fd_and_exit
    jmp     the_code
```

Now tiny Mandelbrot script takes 21 times(!) less time and only 0.8 sec:
```
real	0m0,814s
user	0m0,810s
sys	0m0,004s
```

## Interpreter with optimizations

Implementation above looks like fastest interpreter, but in fact it could be faster :)

Look at [hello.bf](https://github.com/dkozyr/brainfuck/blob/main/examples/hello.bf) script again. There are some operands repeat several times and could be replaced with only one 1 CPU instruction:

|  Script   |  ASM equivalent | Optimization |
|:---------:|:----------------|:----------------------|
|   +++++   | inc byte [RBX]<br>inc byte [RBX]<br>inc byte [RBX]<br>inc byte [RBX]<br>inc byte [RBX] | add byte [RBX], 5 |
|    <<<    | dec RBX<br>dec RBX<br>dec RBX | sub RBX, 3 |
|   >+>+<<  | inc RBX<br>inc byte [RBX]<br>inc RBX<br>inc byte [RBX] | inc byte [RBX+1]<br>inc byte [RBX+2] |
|    [-]    | ...             | mov byte [RBX], 0     |

Now we have to control pointer position to reduce useless movements, but such optimizations give us 3x speed-up.

[fast.asm](https://github.com/dkozyr/brainfuck/blob/main/nasm_experiments/fast.asm)
```
real	0m0,235s
user	0m0,235s
sys	0m0,000s
```

## Final version:

Further [optimizations](http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html) are possible, but we need higher language for brainfuck code interpreting and I choose C++ (C++17).

[core/Optimizer.cpp](https://github.com/dkozyr/brainfuck/blob/main/core/Optimizer.cpp) is most challenging part, but finally it works and Mandelbrot (tiny) script is executed for only ~160ms (100x faster than my first interpreter).

```
real	0m0,163s
user	0m0,163s
sys	0m0,000s
```

## Fastest Brainfuck Interpreter

> Perfection is not attainable. But if we chase perfection, we can catch excellence.

>   -- Vince Lombardi

Tritium is still the best:

```
real	0m0,145s
user	0m0,141s
sys	0m0,004s
```

https://github.com/rdebath/Brainfuck/tree/master/tritium

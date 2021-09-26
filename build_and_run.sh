filename="interpreter_with_compiler"

rm -f brainfuck
nasm -f elf64 $filename.asm && ld -n -s $filename.o -o brainfuck
rm -f $filename.o

stat --printf="%s" ./brainfuck
echo \ bytes

echo "Script: Hello World"
time ./brainfuck ./examples/hello.bf

echo ; echo "Script: Benchmark #1"
time ./brainfuck ./examples/bench-1.bf
#time ./brainfuck ./examples/bench-2.bf

echo ; echo "Script: PI digits" #3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706
echo "8" > digits.in
time ./brainfuck ./examples/pi-digits.bf < digits.in
rm -f digits.in

time ./brainfuck ./examples/chess.bf
time ./brainfuck ./examples/hanoi.bf

time ./brainfuck ./examples/mandelbrot-tiny.bf
time ./brainfuck ./examples/mandelbrot.bf
#time ./brainfuck ./examples/mandelbrot-huge.bf
#time ./brainfuck ./examples/mandelbrot-titannic.bf
#time ./brainfuck ./examples/mandelbrot-extreme.bf

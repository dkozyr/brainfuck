filename="fast"

rm -f brainfuck
nasm -f elf64 $filename.asm && ld -n -s $filename.o -o brainfuck
rm -f $filename.o

stat --printf="%s" ../brainfuck
echo \ bytes

echo "Script: Hello World"
time ./brainfuck ../examples/hello.bf

echo ; echo "Script: Benchmark #1"
time ./brainfuck ../examples/bench-1.bf

echo ; echo "Script: Benchmark #2"
time ./brainfuck ../examples/bench-2.bf

echo ; echo "Script: PI digits" #3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706
echo "8" > digits.in
time ./brainfuck ../examples/pi-digits.bf < digits.in
rm -f digits.in

echo ; echo "Script: Factor"
echo "133333333333337" > factor.in
time ./brainfuck ../examples/factor.bf < factor.in
rm -f factor.in

echo ; echo "Script: Chess"
time ./brainfuck ../examples/chess.bf

echo ; echo "Script: Mandelbrot (tiny)"
time ./brainfuck ../examples/mandelbrot-tiny.bf

echo ; echo "Script: Mandelbrot"
time ./brainfuck ../examples/mandelbrot.bf

echo ; echo "Script: Mandelbrot (huge)"
time ./brainfuck ../examples/mandelbrot-huge.bf

echo ; echo "Script: Mandelbrot (titannic)"
time ./brainfuck ../examples/mandelbrot-titannic.bf

echo ; echo "Script: Mandelbrot (extreme)"
time ./brainfuck ../examples/mandelbrot-extreme.bf

#echo ; echo "Script: Hanoi"
#time ./brainfuck ../examples/hanoi.bf

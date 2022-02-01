rm -f build
mkdir build
cd build

cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja

./bin/core-tests

echo "Script: Hello World"
time ./bin/brainfuck --path ../examples/hello.bf

echo ; echo "Script: Benchmark #1"
time ./bin/brainfuck --path ../examples/bench-1.bf

echo ; echo "Script: Benchmark #2"
time ./bin/brainfuck --path ../examples/bench-2.bf

echo ; echo "Script: PI digits" #3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706
echo "8" > digits.in
time ./bin/brainfuck --path ../examples/pi-digits.bf < digits.in
rm -f digits.in

echo ; echo "Script: Factor"
echo "133333333333337" > factor.in
time ./bin/brainfuck --path ../examples/factor.bf < factor.in
rm -f factor.in

echo ; echo "Script: Chess"
time ./bin/brainfuck --path ../examples/chess.bf

echo ; echo "Script: Mandelbrot (tiny)"
time ./bin/brainfuck --path ../examples/mandelbrot-tiny.bf

echo ; echo "Script: Mandelbrot"
time ./bin/brainfuck --path ../examples/mandelbrot.bf

echo ; echo "Script: Mandelbrot (huge)"
time ./bin/brainfuck --path ../examples/mandelbrot-huge.bf

echo ; echo "Script: Mandelbrot (titannic)"
time ./bin/brainfuck --path ../examples/mandelbrot-titannic.bf

echo ; echo "Script: Mandelbrot (extreme)"
time ./bin/brainfuck --path ../examples/mandelbrot-extreme.bf

#echo ; echo "Script: Hanoi"
#time ./bin/brainfuck --path ../examples/hanoi.bf

#include "Compiler.h"

#include <boost/iostreams/device/mapped_file.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    boost::iostreams::mapped_file mmap("../examples/mandelbrot-tiny.bf", boost::iostreams::mapped_file::readonly);
    // boost::iostreams::mapped_file mmap("../examples/mandelbrot.bf", boost::iostreams::mapped_file::readonly);
    // boost::iostreams::mapped_file mmap("../examples/bench-1.bf", boost::iostreams::mapped_file::readonly);
    // boost::iostreams::mapped_file mmap("../examples/bench-2.bf", boost::iostreams::mapped_file::readonly);
    // boost::iostreams::mapped_file mmap("../examples/hello.bf", boost::iostreams::mapped_file::readonly);
    auto begin = mmap.const_data();
    auto end = begin + mmap.size();
    // std::cout << begin << std::endl;

    Compiler bf;
    bf.Process(begin, end);
    // bf.Execute();
    bf.ExecuteOptimized();

    return 0;
}

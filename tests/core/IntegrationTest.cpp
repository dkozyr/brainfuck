#include "Compiler.h"

#include <gtest/gtest.h>

TEST(Integration, HelloWorld) {
    Compiler bf(std::string{EXAMPLES_PATH} + "/hello.bf");
    bf.Execute();
    bf.ExecuteOptimized();
}
TEST(Integration, Bench1) {
    Compiler bf(std::string{EXAMPLES_PATH} + "/bench-1.bf");
    bf.Execute();
    bf.ExecuteOptimized();
    std::cout << std::endl;
    // DebugScript(bf.GetProgram());
}

TEST(Integration, Bench2) {
    Compiler bf(std::string{EXAMPLES_PATH} + "/bench-2.bf");
    bf.Execute();
    bf.ExecuteOptimized();
}

TEST(Integration, MandelbrotTiny) {
    Compiler bf(std::string{EXAMPLES_PATH} + "/mandelbrot-tiny.bf");
    bf.ExecuteOptimized();
}

TEST(Integration, DISABLED_pi) {
    Compiler bf(std::string{EXAMPLES_PATH} + "/pi-digits.bf");
    bf.ExecuteOptimized();
}

TEST(Integration, DISABLED_e) {
    Compiler bf(std::string{EXAMPLES_PATH} + "/e.bf");
    bf.ExecuteOptimized(1'000'000);
}

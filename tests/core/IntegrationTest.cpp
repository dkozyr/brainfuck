#include "../../Compiler.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>

//TODO: refactor it
std::string ReadFile(const boost::filesystem::path& path) {
    std::ifstream file(path.string());
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

void DebugScript(const std::vector<Operand>& program) {
    auto optimized = Optimizer::ProcessOffsets(program);
    DebugProgram(program);
    DebugProgram(optimized);
}

TEST(Integration, HelloWorld) {
    auto script = ReadFile(std::string{EXAMPLES_PATH} + "/hello.bf");
    Compiler bf;
    bf.Process(script.data(), script.data() + script.size());
    bf.Execute();
}
TEST(Integration, Bench1) {
    auto script = ReadFile(std::string{EXAMPLES_PATH} + "/bench-1.bf");
    Compiler bf;
    bf.Process(script.data(), script.data() + script.size());
    bf.Execute();
    std::cout << std::endl;
    // DebugScript(bf.GetProgram());
}

TEST(Integration, Bench2) {
    auto script = ReadFile(std::string{EXAMPLES_PATH} + "/bench-2.bf");
    Compiler bf;
    bf.Process(script.data(), script.data() + script.size());
    bf.Execute();
    // std::cout << std::endl;
}

TEST(Integration, DISABLED_MandelbrotTiny) {
    auto script = ReadFile(std::string{EXAMPLES_PATH} + "/mandelbrot-tiny.bf");
    Compiler bf;
    bf.Process(script.data(), script.data() + script.size());
    bf.Execute();
    std::cout << std::endl;
}

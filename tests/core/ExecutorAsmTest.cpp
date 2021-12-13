#include "ExecutorAsm.h"

#include <gtest/gtest.h>

TEST(ExecutorAsmTest, Increment) {
    std::vector<Operand> program = {
        Assign{.value = 1},
        PtrAdd{.count = 1},
        Assign{.value = 0},
        PtrAdd{.count = 5},
        Assign{.value = 42},
        PtrSub{.count = 6},
        Add{.count = 16},
        PtrAdd{.count = 1},
        Add{.count = 1},
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    auto data = executor.GetData();
    ASSERT_EQ(17, (size_t)data[0]);
    ASSERT_EQ(1, (size_t)data[1]);
    ASSERT_EQ(42, (size_t)data[6]);
}

TEST(ExecutorAsmTest, Decrement) {
    std::vector<Operand> program = {
        Assign{.value = 17},
        PtrAdd{.count = 1},
        Assign{.value = 10},
        PtrAdd{.count = 5},
        Assign{.value = 42},
        Sub{.count = 13, .offset = -6},
        Sub{.count = 1, .offset = -5},
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    auto data = executor.GetData();
    ASSERT_EQ(4, (size_t)data[0]);
    ASSERT_EQ(9, (size_t)data[1]);
    ASSERT_EQ(42, (size_t)data[6]);
}

TEST(ExecutorAsmTest, Assign) {
    std::vector<Operand> program = {
        Assign{.value = 17, .offset = 0},
        Assign{.value = 18, .offset = 1},
        Assign{.value = 19, .offset = 2},
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    auto data = executor.GetData();
    ASSERT_EQ(17, (size_t)data[0]);
    ASSERT_EQ(18, (size_t)data[1]);
    ASSERT_EQ(19, (size_t)data[2]);
}

TEST(ExecutorAsmTest, Mul) {
    std::vector<Operand> program = {
        Assign{.value = 2, .offset = 0},
        Assign{.value = 3, .offset = 1},
        Assign{.value = 4, .offset = 2},
        Mul{.offset = 0, .value = 5, .factor_offset = 1},
        Mul{.offset = 1, .value = 7, .factor_offset = 2}
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    auto data = executor.GetData();
    ASSERT_EQ(2 + 3 * 5, (size_t)data[0]);
    ASSERT_EQ(3 + 7 * 4, (size_t)data[1]);
}

TEST(ExecutorAsmTest, MulShift) {
    std::vector<Operand> program = {
        Assign{.value = 2, .offset = 0},
        Assign{.value = 3, .offset = 1},
        Assign{.value = 4, .offset = 2},
        Mul{.offset = 1, .value = 2, .factor_offset = 2},
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    auto data = executor.GetData();
    ASSERT_EQ(2, (size_t)data[0]);
    ASSERT_EQ(3 + 2 * 4, (size_t)data[1]);
}

TEST(ExecutorAsmTest, Output) {
    std::vector<Operand> program = {
        Assign{.value = 'N', .offset = 0},
        Assign{.value = 'Y', .offset = 1},
        Output{.offset = 1},
        PtrAdd{.count = 10},
        Assign{.value = 'e', .offset = -9},
        Assign{.value = 's', .offset = -10},
        Output{.offset = -9},
        Output{.offset = -10},
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    std::cout << std::endl;
}

TEST(ExecutorAsmTest, While) {
    std::vector<Operand> program = {
        Assign{.value = 'A', .offset = 0},
        Assign{.value = 10, .offset = 1},
        WhileBegin{.offset = 1},
        Sub{.count = 1, .offset = 1},
        Output{.offset = 0},
        Add{.count = 1, .offset = 0},
        WhileEnd{.offset = 1},
    };
    ExecutorAsm executor(program);
    executor.RunWithoutOptimization();
    std::cout << std::endl;
}

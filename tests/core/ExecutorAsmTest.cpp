#include "../../core/ExecutorAsm.h"

#include <gtest/gtest.h>

#include <iostream>

class ExecutorAsmTest : public ::testing::Test {
public:
    ExecutorAsmTest()
        : _data(kBrainfuckDataSize, 0) {
    }

protected:
    std::vector<uint8_t> _data;
};

TEST_F(ExecutorAsmTest, Increment) {
    std::vector<Operand> program = {
        Assign{.value = 1},
        PtrAdd{.count = 1},
        Assign{.value = 0},
        PtrAdd{.count = 5},
        Assign{.value = 42},
        Add{.count = 16, .offset = -6},
        Add{.count = 1, .offset = -5},
    };
    ExecutorAsm::Run(program, _data.data());
    ASSERT_EQ(17, (size_t)_data[0]);
    ASSERT_EQ(1, (size_t)_data[1]);
    ASSERT_EQ(42, (size_t)_data[6]);
}

TEST_F(ExecutorAsmTest, Decrement) {
    std::vector<Operand> program = {
        Assign{.value = 17},
        PtrAdd{.count = 1},
        Assign{.value = 10},
        PtrAdd{.count = 5},
        Assign{.value = 42},
        Sub{.count = 13, .offset = -6},
        Sub{.count = 1, .offset = -5},
    };
    ExecutorAsm::Run(program, _data.data());
    ASSERT_EQ(4, (size_t)_data[0]);
    ASSERT_EQ(9, (size_t)_data[1]);
    ASSERT_EQ(42, (size_t)_data[6]);
}

TEST_F(ExecutorAsmTest, Assign) {
    std::vector<Operand> program = {
        Assign{.value = 17, .offset = 0},
        Assign{.value = 18, .offset = 1},
        Assign{.value = 19, .offset = 2},
    };
    ExecutorAsm::Run(program, _data.data());
    ASSERT_EQ(17, (size_t)_data[0]);
    ASSERT_EQ(18, (size_t)_data[1]);
    ASSERT_EQ(19, (size_t)_data[2]);
}

TEST_F(ExecutorAsmTest, Mul) {
    std::vector<Operand> program = {
        Assign{.value = 2, .offset = 0},
        Assign{.value = 3, .offset = 1},
        Assign{.value = 4, .offset = 2},
        Mul{.offset = 0, .value = 5, .factor_offset = 1},
        Mul{.offset = 1, .value = 7, .factor_offset = 2}
    };
    ExecutorAsm::Run(program, _data.data());
    ASSERT_EQ(2 + 3 * 5, (size_t)_data[0]);
    ASSERT_EQ(3 + 7 * 4, (size_t)_data[1]);
}

TEST_F(ExecutorAsmTest, MulShift) {
    std::vector<Operand> program = {
        Assign{.value = 2, .offset = 0},
        Assign{.value = 3, .offset = 1},
        Assign{.value = 4, .offset = 2},
        Mul{.offset = 1, .value = 2, .factor_offset = 2},
    };
    ExecutorAsm::Run(program, _data.data());
    ASSERT_EQ(2, (size_t)_data[0]);
    ASSERT_EQ(3 + 2 * 4, (size_t)_data[1]);
}

TEST_F(ExecutorAsmTest, Output) {
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
    ExecutorAsm::Run(program, _data.data());
    std::cout << std::endl;
}

TEST_F(ExecutorAsmTest, While) {
    std::vector<Operand> program = {
        Assign{.value = 'A', .offset = 0},
        Assign{.value = 10, .offset = 1},
        WhileBegin{.offset = 1},
        Sub{.count = 1, .offset = 1},
        Output{.offset = 0},
        Add{.count = 1, .offset = 0},
        WhileEnd{.offset = 1},
    };
    ExecutorAsm::Run(program, _data.data());
    std::cout << std::endl;
}

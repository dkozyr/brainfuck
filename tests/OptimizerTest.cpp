#include "../Optimizer.h"

#include <gtest/gtest.h>

TEST(Optimizer, Basic) {
    std::vector<Operand> program = {
        Add{.count = 1},
        PtrAdd{.count = 1},
        Sub{.count = 1},
        PtrSub{.count = 1},
        Add{.count = 1},
    };

    std::vector<Operand> target = {
        Sub{.count = 1, .offset = 1},
        Add{.count = 2, .offset = 0},
    };

    auto optimized = Optimizer::ProcessOffsets(program);
    ASSERT_EQ(2, optimized.size());
    EXPECT_EQ(target, optimized);
}

TEST(Optimizer, While) {
    std::vector<Operand> program = {
        Add{.count = 1},
        PtrAdd{.count = 1},
        WhileBegin{},
        Sub{.count = 1},
        PtrAdd{.count = 1},
        WhileBegin{},
        PtrSub{.count = 1},
        Add{.count = 1},
        WhileEnd{},
        WhileEnd{},
    };

    std::vector<Operand> target = {
        Add{.count = 1, .offset = 0},
        WhileBegin{.offset = 1},
        Sub{.count = 1, .offset = 1},
        WhileBegin{.offset = 2},
        Add{.count = 1, .offset = 1},
        PtrSub{.count = 1},
        WhileEnd{.offset = 2},
        PtrAdd{.count = 1},
        WhileEnd{.offset = 1},
    };

    auto optimized = Optimizer::ProcessOffsets(program);
    ASSERT_EQ(target.size(), optimized.size());
    EXPECT_EQ(target, optimized);
}

TEST(Optimizer, Set) {
    std::vector<Operand> program = {
        WhileBegin{},
        Sub{.count = 1},
        PtrAdd{.count = 1},
        Set{.value = 10},
        PtrSub{.count = 1},
        WhileEnd{},
        Set{.value = 42},
    };

    std::vector<Operand> target = {
        WhileBegin{},
        Sub{.count = 1},
        Set{.value = 10, .offset = 1},
        WhileEnd{},
        Set{.value = 42},
    };

    auto optimized = Optimizer::ProcessOffsets(program);
    // DebugProgram(program);
    // DebugProgram(optimized);
    ASSERT_EQ(target.size(), optimized.size());
    EXPECT_EQ(target, optimized);
}

TEST(Optimizer, Mul) {
    std::vector<Operand> program = {
        WhileBegin{},
        PtrAdd{.count = 1},
        Add{.count = 1},
        PtrAdd{.count = 1},
        Add{.count = 2},
        PtrSub{.count = 2},
        Sub{.count = 1},
        WhileEnd{},
    };

    std::vector<Operand> target = {
        Mul{.offset = 2, .value = 2, .factor_offset = 0},
        Mul{.offset = 1, .value = 1, .factor_offset = 0},
        Set{.value = 0, .offset = 0},
    };

    auto optimized = Optimizer::ProcessOffsets(program);
    ASSERT_EQ(target.size(), optimized.size());
    EXPECT_EQ(target, optimized);
}

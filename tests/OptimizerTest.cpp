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

#pragma once

#include "Optimizer.h"
#include "BracketMap.h"

#include <stack>

class Executor {
public:
    static void Run(const std::vector<Operand>& program) {
        std::vector<uint8_t> data(kBrainfuckDataSize, 0);

        const auto optimized = Optimizer::ProcessOffsets(program);
        const auto bracket_map = CreateBracketMap(optimized);

        size_t idx = 0;
        int32_t offset = 0;
        while(idx < optimized.size()) {
            const auto& operand = optimized.at(idx);
            std::visit(overloaded {
                [&](const Add& x) { data[offset + x.offset] += x.count; },
                [&](const Sub& x) { data[offset + x.offset] -= x.count; },
                [&](const Set& x) { data[offset + x.offset] = x.value; },
                [&](const PtrAdd& x) { offset += x.count; },
                [&](const PtrSub& x) { offset -= x.count; },
                [&](const WhileBegin& x) {
                    if(data[offset + x.offset] == 0) {
                        idx = bracket_map.left.at(idx);
                    }
                },
                [&](const WhileEnd& x) {
                    if(data[offset + x.offset] != 0) {
                        idx = bracket_map.right.at(idx);
                    }
                },
                [&](const Output& x) { std::cout << data[offset + x.offset]; },
                [&](const Input& x) { std::cin >> data[offset + x.offset]; },
            }, operand);
            ++idx;
        }
    }

private:
    static BracketMap CreateBracketMap(const std::vector<Operand>& program) {
        BracketMap bracket_map;
        std::stack<size_t> brackets_stack;
        for(size_t i = 0; i < program.size(); ++i) {
            const auto& operand = program.at(i);
            std::visit(overloaded {
                [&](const WhileBegin&) {
                    brackets_stack.push(i);
                },
                [&](const WhileEnd&) {
                    bracket_map.insert(BracketMapElement(brackets_stack.top(), i));
                    brackets_stack.pop();
                },
                [&](const auto&) { /*do nothing*/ },
            }, operand);
        }
        return bracket_map;
    }
};

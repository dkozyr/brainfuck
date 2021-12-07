#pragma once

#include "Operand.h"
#include "BracketMap.h"

#include <vector>
#include <stack>

class Executor {
public:
    static void Run(const std::vector<Operand>& program) {
        std::vector<uint8_t> data(kBrainfuckDataSize, 0);

        const auto bracket_map = CreateBracketMap(program);

        size_t idx = 0;
        size_t offset = 0;
        while(idx < program.size()) {
            const auto& operand = program.at(idx);
            std::visit(overloaded {
                [&](const Add& x) { data[offset] += x.count; },
                [&](const Sub& x) { data[offset] -= x.count; },
                [&](const Set& x) { data[offset] = x.value; },
                [&](const PtrAdd& x) { offset += x.count; },
                [&](const PtrSub& x) { offset -= x.count; },
                [&](const WhileBegin&) {
                    if(data[offset] == 0) {
                        idx = bracket_map.left.at(idx);
                    }
                },
                [&](const WhileEnd&) {
                    if(data[offset] != 0) {
                        idx = bracket_map.right.at(idx);
                    }
                },
                [&](const Output&) { std::cout << data[offset]; },
                [&](const Input&) { std::cin >> data[offset]; },
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

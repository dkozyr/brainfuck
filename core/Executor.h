#pragma once

#include "Optimizer.h"
#include "BracketMap.h"

#include <stack>

class Executor {
public:
    Executor(const std::vector<Operand>& program, size_t data_size = kDefaultDataSize)
        : _program(Optimizer::ProcessOffsets(program))
        , _data(data_size, 0) {
    }

    void Run() {
        const auto bracket_map = CreateBracketMap(_program);

        size_t idx = 0;
        int32_t offset = 0;
        while(idx < _program.size()) {
            const auto& operand = _program.at(idx);
            std::visit(overloaded {
                [&](const Add& x) { _data[offset + x.offset] += x.count; },
                [&](const Sub& x) { _data[offset + x.offset] -= x.count; },
                [&](const Assign& x) { _data[offset + x.offset] = x.value; },
                [&](const Mul& x) {
                    auto factor = _data[offset + x.factor_offset];
                    _data[offset + x.offset] += x.value * factor;
                },
                [&](const PtrAdd& x) { offset += x.count; },
                [&](const PtrSub& x) { offset -= x.count; },
                [&](const WhileBegin& x) {
                    if(_data[offset + x.offset] == 0) {
                        idx = bracket_map.left.at(idx);
                    }
                },
                [&](const WhileEnd& x) {
                    if(_data[offset + x.offset] != 0) {
                        idx = bracket_map.right.at(idx);
                    }
                },
                [&](const Output& x) { std::cout << _data[offset + x.offset]; },
                [&](const Input& x) { _data[offset + x.offset] = getchar(); },
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

private:
    const std::vector<Operand> _program;
    std::vector<uint8_t> _data;
};

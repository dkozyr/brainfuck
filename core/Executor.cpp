#include "Executor.h"
#include "Optimizer.h"

#include <stack>

Executor::Executor(const std::vector<Operand>& program, size_t data_size)
    : _program(Optimizer::Process(program))
    , _data(data_size, 0) {
}

void Executor::Run() {
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

//In fact it could be refactored with unordered_map, but let's leave bimap here
BracketMap Executor::CreateBracketMap(const std::vector<Operand>& program) {
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

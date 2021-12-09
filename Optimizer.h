#pragma once

#include "Operand.h"

#include <stack>
#include <unordered_map>

class Optimizer {
public:
    static std::vector<Operand> ProcessOffsets(const std::vector<Operand>& program) {
        const auto size = program.size();

        std::vector<Operand> optimized;
        std::unordered_map<int32_t, int64_t> offset_to_delta;
        std::stack<int32_t> while_begin_offset;
        int32_t offset = 0, offset_prev = 0;
        for(size_t i = 0; i < size; ++i) {
            const auto& operand = program[i];
            std::visit(overloaded {
                [&](const Add& x) { offset_to_delta[offset] += x.count; },
                [&](const Sub& x) { offset_to_delta[offset] -= x.count; },
                [&](const PtrAdd& x) { offset += x.count; },
                [&](const PtrSub& x) { offset -= x.count; },
                [&](const Set& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Set{.value = x.value, .offset = offset});
                },
                [&](const WhileBegin& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(WhileBegin{.offset = offset});

                    while_begin_offset.push(offset);
                },
                [&](const WhileEnd& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);

                    offset_prev = while_begin_offset.top();
                    while_begin_offset.pop();
                    PushDeltaOperand(offset, offset_prev, optimized);
                    offset = offset_prev;

                    optimized.push_back(WhileEnd{.offset = offset});
                },
                [&](const Output& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Output{.offset = offset});
                },
                [&](const Input& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Input{.offset = offset});
                }
            }, operand);
        }
        PushArithmeticOperands(offset_to_delta, optimized);
        return optimized;
    }

private:
    static void PushArithmeticOperands(std::unordered_map<int32_t, int64_t>& offset_to_delta, std::vector<Operand>& optimized) {
        for(auto& value: offset_to_delta) {
            if(value.second > 0) {
                optimized.push_back(Add{.count = static_cast<size_t>(value.second), .offset = value.first});
            } else if(value.second < 0) {
                optimized.push_back(Sub{.count = static_cast<size_t>(-value.second), .offset = value.first});
            } else {
                //do nothing
            }
        }
        offset_to_delta.clear();
    }

    static void PushDeltaOperand(const int32_t offset, const int32_t offset_prev, std::vector<Operand>& optimized) {
        if(offset != offset_prev) {
            const auto ptr_delta = offset - offset_prev;
            if(ptr_delta > 0) {
                optimized.push_back(PtrAdd{.count = static_cast<size_t>(ptr_delta)});
            } else {
                optimized.push_back(PtrSub{.count = static_cast<size_t>(-ptr_delta)});
            }
        }
    }
};

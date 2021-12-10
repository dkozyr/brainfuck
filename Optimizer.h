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
        int32_t offset = 0, offset_prev = 0;
        for(size_t idx = 0; idx < size; ++idx) {
            const auto& operand = program[idx];
            std::visit(overloaded {
                [&](const Add& x) { offset_to_delta[offset] += x.count; },
                [&](const Sub& x) { offset_to_delta[offset] -= x.count; },
                [&](const PtrAdd& x) { offset += x.count; },
                [&](const PtrSub& x) { offset -= x.count; },
                [&](const Set& x) {
                    offset_to_delta[offset] = 0;
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Set{.value = x.value, .offset = offset});
                },
                [&](const WhileBegin& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);
                    
                    idx++;
                    auto while_optimized = ProcessWhile(program, idx, offset);
                    optimized.insert(optimized.end(), while_optimized.begin(), while_optimized.end());
                },
                [&](const WhileEnd& x) {
                    throw -1;
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
    static std::vector<Operand> ProcessWhile(const std::vector<Operand>& program, size_t& idx, int32_t offset) {
        const auto size = program.size();
        const auto while_begin_offset = offset;

        std::vector<Operand> optimized = {WhileBegin{.offset = offset}};
        std::unordered_map<int32_t, int64_t> offset_to_delta;
        bool optimize = true;
        while(idx < size) {
            const auto& operand = program[idx];
            auto while_end = std::visit(overloaded {
                [&](const Add& x) { offset_to_delta[offset] += x.count; return false; },
                [&](const Sub& x) { offset_to_delta[offset] -= x.count; return false; },
                [&](const PtrAdd& x) { offset += x.count; return false; },
                [&](const PtrSub& x) { offset -= x.count; return false; },
                [&](const Set& x) {
                    offset_to_delta[offset] = 0;
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Set{.value = x.value, .offset = offset});
                    return false;
                },
                [&](const WhileBegin& x) {
                    PushArithmeticOperands(offset_to_delta, optimized);

                    optimize = false;
                    idx++;
                    auto while_optimized = ProcessWhile(program, idx, offset);
                    optimized.insert(optimized.end(), while_optimized.begin(), while_optimized.end());
                    return false;
                },
                [&](const WhileEnd& x) {
                    if(optimize) {
                        //TODO
                    }

                    PushArithmeticOperands(offset_to_delta, optimized);
                    PushDeltaOperand(offset, while_begin_offset, optimized);

                    offset = while_begin_offset;
                    optimized.push_back(WhileEnd{.offset = offset});
                    return true;
                },
                [&](const Output& x) {
                    optimize = false;
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Output{.offset = offset});
                    return false;
                },
                [&](const Input& x) {
                    optimize = false;
                    PushArithmeticOperands(offset_to_delta, optimized);
                    optimized.push_back(Input{.offset = offset});
                    return false;
                }
            }, operand);

            if(while_end) {
                return optimized;
            }
            idx++;
        }
        throw -1;
    }

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

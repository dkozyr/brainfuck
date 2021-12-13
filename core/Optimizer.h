#pragma once

#include "Operand.h"

#include <unordered_map>

class Optimizer {
public:
    static std::vector<Operand> Process(const std::vector<Operand>& program);

private:
    static std::vector<Operand> ProcessWhile(const std::vector<Operand>& program, size_t& idx, int32_t offset);
    static void PushArithmeticOperands(std::unordered_map<int32_t, int32_t>& offset_to_delta, std::vector<Operand>& optimized);
    static void PushDeltaOperand(const int32_t offset, const int32_t offset_prev, std::vector<Operand>& optimized);
};

#pragma once

#include "Operand.h"

#include <vector>

class Compiler {
public:
    Compiler() = default;

    void Process(const char* begin, const char* end);
    void ExecuteOptimized();
    void Execute();

private:
    void Process(const Operand& operand);

    template<typename TSame, typename TOpposite>
    void CheckOperandWithPreviousAndInsert(const Operand& current, Operand& previous) {
        if(auto v = std::get_if<TSame>(&previous); v) {
            v->count++;
        } else if(auto v = std::get_if<TOpposite>(&previous); v) {
            v->count--;
            if(v->count == 0) {
                _program.pop_back();
            }
        } else {
            _program.push_back(current);
        }
    }

private:
    std::vector<Operand> _program;
};

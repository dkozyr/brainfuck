#pragma once

#include "Executor.h"
#include "ExecutorAsm.h"

#include <boost/iostreams/device/mapped_file.hpp>

class Compiler {
public:
    Compiler(const std::string& script_path);

    void ExecuteOptimized(size_t data_size = kDefaultDataSize);
    void Execute(size_t data_size = kDefaultDataSize);

    const std::vector<Operand>& GetProgram() const { return _program; }

private:
    void Process(const char* begin, const char* end);
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

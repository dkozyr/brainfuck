#pragma once

#include "Operand.h"
#include "BracketMap.h"

#include <vector>

class Compiler {
public:
    Compiler();

    void Process(const char* begin, const char* end);
    void ExecuteOptimized();
    void Execute();

private:
    void Process(const Operand& operand);
    void ProcessBrackets();

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

    static uint8_t* PrepareGeneratedCodeEntryPoint(uint8_t* ptr);
    static void ExecuteGeneratedCode(uint8_t* entry_point, uint8_t* data);

private:
    struct OperandContext {
        uint8_t* ptr;
        size_t size = 0;
    };

    template<typename TFunc>
    OperandContext WriteOperand(uint8_t* ptr, uint32_t jump_offset, const TFunc& operand_writer) {
        return OperandContext{
            .ptr = ptr,
            .size = operand_writer(ptr, jump_offset)
        };
    }

private:
    static constexpr auto kAsmCodeSize = 1024 * 1024;
    static constexpr auto kPageSize = 65536;

    std::vector<Operand> _program;
    BracketMap _bracket_map;

    std::vector<uint8_t> _asm_code;
    std::vector<uint8_t> _data;
};

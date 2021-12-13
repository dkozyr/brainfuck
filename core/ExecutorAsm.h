#pragma once

#include "Optimizer.h"

class ExecutorAsm {
public:
    ExecutorAsm(const std::vector<Operand>& program, size_t data_size = kDefaultDataSize);

    void Run();
    void RunWithoutOptimization();

    const std::vector<uint8_t>& GetData() const { return _data; }

private:
    static void Run(const std::vector<Operand>& program, uint8_t* data);

    static uint8_t* PrepareGeneratedCodeEntryPoint(uint8_t* ptr);
    static void ExecuteGeneratedCode(uint8_t* entry_point, uint8_t* data);

private:
    static constexpr auto kAsmCodeSize = 1024 * 1024;
    static constexpr auto kPageSize = 65536;

    struct OperandContext {
        uint8_t* ptr;
        size_t size = 0;
        int32_t offset = 0;
    };

    template<typename TFunc>
    static OperandContext WriteOperand(uint8_t* ptr, uint32_t jump_offset, const TFunc& operand_writer) {
        return OperandContext{
            .ptr = ptr,
            .size = operand_writer(ptr, jump_offset)
        };
    }

    template<typename TFunc>
    static OperandContext WriteOperand(uint8_t* ptr, int32_t offset, uint32_t jump_offset, const TFunc& operand_writer_with_offset) {
        return OperandContext{
            .ptr = ptr,
            .size = operand_writer_with_offset(ptr, offset, jump_offset),
            .offset = offset
        };
    }

private:
    const std::vector<Operand>& _program;
    std::vector<uint8_t> _data;
};

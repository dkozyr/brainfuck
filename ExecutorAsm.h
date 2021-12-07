#pragma once

#include "Operand.h"
#include "operand_writer/OperandWriter.h"

#include <stack>

class ExecutorAsm {
public:
    static void Run(const std::vector<Operand>& program) {
        std::vector<uint8_t> asm_code(kAsmCodeSize + kPageSize);
        std::vector<uint8_t> data(kBrainfuckDataSize, 0);

        auto entry_point = PrepareGeneratedCodeEntryPoint(asm_code.data());
        auto ptr = entry_point;
        ptr += OperandWriterProlog(ptr);

        std::stack<OperandContext> while_ctx;
        for(const auto& operand: program) {
            std::visit(overloaded {
                [&](const Add& x) {
                    if(x.count == 1) {
                        ptr += OperandWriterInc(ptr);
                    } else {
                        ptr += OperandWriterAdd(ptr, x.count);
                    }
                },
                [&](const Sub& x) {
                    if(x.count == 1) {
                        ptr += OperandWriterDec(ptr);
                    } else {
                        ptr += OperandWriterSub(ptr, x.count);
                    }
                },
                [&](const Set& x) {
                    ptr += OperandWriterSet(ptr, x.value);
                },
                [&](const PtrAdd& x) {
                    if(x.count == 1) {
                        ptr += OperandWriterPtrInc(ptr);
                    } else {
                        ptr += OperandWriterPtrAdd(ptr, x.count);
                    }
                },
                [&](const PtrSub& x) {
                    if(x.count == 1) {
                        ptr += OperandWriterPtrDec(ptr);
                    } else {
                        ptr += OperandWriterPtrSub(ptr, x.count);
                    }
                },
                [&](const WhileBegin&) {
                    auto ctx_begin = WriteOperand(ptr, 0, OperandWriterWhileBegin);
                    ptr += ctx_begin.size;
                    while_ctx.push(ctx_begin);
                },
                [&](const WhileEnd&) {
                    auto ctx_end = WriteOperand(ptr, 0, OperandWriterWhileEnd);
                    ptr += ctx_end.size;
                    {
                        auto ctx_begin = while_ctx.top();
                        while_ctx.pop();

                        auto body_ptr = ctx_begin.ptr + ctx_begin.size;
                        WriteOperand(ctx_begin.ptr, static_cast<uint32_t>(ptr - body_ptr), OperandWriterWhileBegin);
                        WriteOperand(ctx_end.ptr, static_cast<uint32_t>(body_ptr - ptr), OperandWriterWhileEnd);
                    }
                },
                [&](const Output&) {
                    ptr += OperandWriterOutput(ptr);
                },
                [&](const Input&) {
                    std::cout << "NOT IMPLEMENTED" << std::endl;
                    throw -1;
                },
            }, operand);
        }

        ptr += OperandWriterRet(ptr);
        ExecuteGeneratedCode(entry_point, data.data());
    }

private:
    static constexpr auto kAsmCodeSize = 1024 * 1024;
    static constexpr auto kPageSize = 65536;

    struct OperandContext {
        uint8_t* ptr;
        size_t size = 0;
    };

    template<typename TFunc>
    static OperandContext WriteOperand(uint8_t* ptr, uint32_t jump_offset, const TFunc& operand_writer) {
        return OperandContext{
            .ptr = ptr,
            .size = operand_writer(ptr, jump_offset)
        };
    }

    static uint8_t* PrepareGeneratedCodeEntryPoint(uint8_t* ptr) {
        auto ptr_aligned = ((reinterpret_cast<size_t>(ptr) + (kPageSize - 1)) / kPageSize) * kPageSize;
        auto entry_point = reinterpret_cast<uint8_t*>(ptr_aligned);
        mprotect(entry_point, kAsmCodeSize, PROT_READ | PROT_WRITE | PROT_EXEC);
        return entry_point;
    }

    static void ExecuteGeneratedCode(uint8_t* entry_point, uint8_t* data) {
        __builtin___clear_cache(entry_point, entry_point + kAsmCodeSize);
        ((void(*)(uint8_t*))entry_point)(data);
    }
};

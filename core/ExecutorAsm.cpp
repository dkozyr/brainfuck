#include "ExecutorAsm.h"
#include "Optimizer.h"
#include "../operand_writer/OperandWriter.h"

#include <stack>

ExecutorAsm::ExecutorAsm(const std::vector<Operand>& program, size_t data_size)
    : _program(program)
    , _data(data_size, 0)
{}

void ExecutorAsm::Run() {
    auto optimized = Optimizer::Process(_program);
    Run(optimized, _data.data());
}

void ExecutorAsm::RunWithoutOptimization() {
    Run(_program, _data.data());
}

void ExecutorAsm::Run(const std::vector<Operand>& program, uint8_t* data) {
    std::vector<uint8_t> asm_code(kAsmCodeSize + kPageSize);
    auto entry_point = PrepareGeneratedCodeEntryPoint(asm_code.data());
    auto ptr = entry_point;
    ptr += OperandWriterProlog(ptr);

    std::stack<OperandContext> while_ctx;
    for(const auto& operand: program) {
        std::visit(overloaded {
            [&](const Add& x) {
                if(x.offset == 0) {
                    if(x.count == 1) {
                        ptr += OperandWriterInc(ptr);
                    } else {
                        ptr += OperandWriterAdd(ptr, x.count);
                    }
                } else {
                    if(x.count == 1) {
                        ptr += OperandWriterIncWithOffset(ptr, x.offset);
                    } else {
                        ptr += OperandWriterAddWithOffset(ptr, x.count, x.offset);
                    }
                }
            },
            [&](const Sub& x) {
                if(x.offset == 0) {
                    if(x.count == 1) {
                        ptr += OperandWriterDec(ptr);
                    } else {
                        ptr += OperandWriterSub(ptr, x.count);
                    }
                } else {
                    if(x.count == 1) {
                        ptr += OperandWriterDecWithOffset(ptr, x.offset);
                    } else {
                        ptr += OperandWriterSubWithOffset(ptr, x.count, x.offset);
                    }
                }
            },
            [&](const Assign& x) {
                if(x.offset == 0) {
                    ptr += OperandWriterAssign(ptr, x.value);
                } else {
                    ptr += OperandWriterAssignWithOffset(ptr, x.value, x.offset);
                }
            },
            [&](const Mul& x) {
                if(x.offset == 0) {
                    switch(x.value) {
                        case 1:
                            ptr += OperandWriterCopy(ptr, x.factor_offset);
                            break;

                        default:
                            ptr += OperandWriterMul(ptr, x.value, x.factor_offset);
                            break;
                    }
                } else {
                    switch(x.value) {
                        case 1:
                            ptr += OperandWriterCopyWithOffset(ptr, x.factor_offset, x.offset);
                            break;

                        case -1:
                            ptr += OperandWriterSubVariables(ptr, x.factor_offset, x.offset);
                            break;

                        case 2:
                            ptr += OperandWriterMulShiftWithOffset(ptr, 1, x.factor_offset, x.offset);
                            break;

                        case 4:
                            ptr += OperandWriterMulShiftWithOffset(ptr, 2, x.factor_offset, x.offset);
                            break;

                        case 8:
                            ptr += OperandWriterMulShiftWithOffset(ptr, 3, x.factor_offset, x.offset);
                            break;

                        default:
                            ptr += OperandWriterMulWithOffset(ptr, x.value, x.factor_offset, x.offset);
                            break;
                    }
                }
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
            [&](const WhileBegin& x) {
                if(x.offset == 0) {
                    auto ctx_begin = WriteOperand(ptr, 0, OperandWriterWhileBegin);
                    ptr += ctx_begin.size;
                    while_ctx.push(ctx_begin);
                } else {
                    auto ctx_begin = WriteOperand(ptr, x.offset, 0, OperandWriterWhileBeginWithOffset);
                    ptr += ctx_begin.size;
                    while_ctx.push(ctx_begin);
                }
            },
            [&](const WhileEnd& x) {
                if(x.offset == 0) {
                    auto ctx_end = WriteOperand(ptr, 0, OperandWriterWhileEnd);
                    ptr += ctx_end.size;
                    {
                        auto ctx_begin = while_ctx.top();
                        while_ctx.pop();

                        auto body_ptr = ctx_begin.ptr + ctx_begin.size;
                        WriteOperand(ctx_begin.ptr, static_cast<uint32_t>(ptr - body_ptr), OperandWriterWhileBegin);
                        WriteOperand(ctx_end.ptr, static_cast<uint32_t>(body_ptr - ptr), OperandWriterWhileEnd);
                    }
                } else {
                    auto ctx_end = WriteOperand(ptr, x.offset, 0, OperandWriterWhileEndWithOffset);
                    ptr += ctx_end.size;
                    {
                        auto ctx_begin = while_ctx.top();
                        while_ctx.pop();

                        auto body_ptr = ctx_begin.ptr + ctx_begin.size;
                        WriteOperand(ctx_begin.ptr, ctx_begin.offset, static_cast<uint32_t>(ptr - body_ptr), OperandWriterWhileBeginWithOffset);
                        WriteOperand(ctx_end.ptr, x.offset, static_cast<uint32_t>(body_ptr - ptr), OperandWriterWhileEndWithOffset);
                    }
                }
            },
            [&](const Output& x) {
                if(x.offset == 0) {
                    ptr += OperandWriterOutput(ptr);
                } else {
                    ptr += OperandWriterOutputWithOffset(ptr, x.offset);
                }
            },
            [&](const Input& x) {
                if(x.offset == 0) {
                    ptr += OperandWriterInput(ptr);
                } else {
                    ptr += OperandWriterInputWithOffset(ptr, x.offset);
                }
            },
        }, operand);
    }

    ptr += OperandWriterRet(ptr);
    ExecuteGeneratedCode(entry_point, data);
}

uint8_t* ExecutorAsm::PrepareGeneratedCodeEntryPoint(uint8_t* ptr) {
    auto ptr_aligned = ((reinterpret_cast<size_t>(ptr) + (kPageSize - 1)) / kPageSize) * kPageSize;
    auto entry_point = reinterpret_cast<uint8_t*>(ptr_aligned);
    mprotect(entry_point, kAsmCodeSize, PROT_READ | PROT_WRITE | PROT_EXEC);
    return entry_point;
}

void ExecutorAsm::ExecuteGeneratedCode(uint8_t* entry_point, uint8_t* data) {
    __builtin___clear_cache(entry_point, entry_point + kAsmCodeSize);
    ((void(*)(uint8_t*))entry_point)(data);
}

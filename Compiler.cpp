#include "Compiler.h"
#include "operand_writer/OperandWriter.h"

#include <stack>

Compiler::Compiler()
    : _asm_code(kAsmCodeSize + kPageSize, 0)
    , _data(30'000, 0) {
}

void Compiler::Process(const char* begin, const char* end) {
    while(begin != end) {
        switch(*begin) {
            case '+': Process(Add{}); break;
            case '-': Process(Sub{}); break;
            case '>': Process(PtrAdd{}); break;
            case '<': Process(PtrSub{}); break;
            case '.': Process(Output{}); break;
            case ',': Process(Input{}); break;
            case '[':
                Process(WhileBegin{});
                break;
            case ']':
                if(_program.size() >= 2) {
                    const auto idx = _program.size() - 2;
                    const auto open = std::get_if<WhileBegin>(&_program[idx]);
                    if(open) {
                        const auto sub = std::get_if<Sub>(&_program[idx + 1]);
                        if(sub && (sub->count == 1)) {
                            _program.pop_back();
                            _program.pop_back();
                            Process(Set{});
                            break;
                        }

                        const auto add = std::get_if<Add>(&_program[idx + 1]);
                        if(add && (add->count == 1)) {
                            _program.pop_back();
                            _program.pop_back();
                            Process(Set{});
                            break;
                        }

                    }
                }
                Process(WhileEnd{});
                break;
        }
        begin++;
    }
}

void Compiler::ProcessBrackets() {
    std::stack<size_t> brackets_stack;
    for(size_t i = 0; i < _program.size(); ++i) {
        const auto& operand = _program.at(i);
        std::visit(overloaded {
            [&](const WhileBegin&) {
                brackets_stack.push(i);
            },
            [&](const WhileEnd&) {
                _bracket_map.insert(BracketMapElement(brackets_stack.top(), i));
                brackets_stack.pop();
            },
            [&](const auto&) { /*do nothing*/ },
        }, operand);
    }
}

void Compiler::ExecuteOptimized() {
    auto entry_point = PrepareGeneratedCodeEntryPoint(_asm_code.data());
    auto ptr = entry_point;
    ptr += OperandWriterProlog(ptr);

    std::stack<OperandContext> while_ctx;
    for(const auto& operand: _program) {
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
    ExecuteGeneratedCode(entry_point, _data.data());
}

void Compiler::Execute() {
    ProcessBrackets();

    size_t idx = 0;
    size_t offset = 0;
    while(idx < _program.size()) {
        const auto& operand = _program.at(idx);
        std::visit(overloaded {
            [&](const Add& x) { _data[offset] += x.count; },
            [&](const Sub& x) { _data[offset] -= x.count; },
            [&](const Set& x) { _data[offset] = x.value; },
            [&](const PtrAdd& x) { offset += x.count; },
            [&](const PtrSub& x) { offset -= x.count; },
            [&](const WhileBegin&) {
                if(_data[offset] == 0) {
                    idx = _bracket_map.left.at(idx);
                }
            },
            [&](const WhileEnd&) {
                if(_data[offset] != 0) {
                    idx = _bracket_map.right.at(idx);
                }
            },
            [&](const Output&) { std::cout << _data[offset]; },
            [&](const Input&) { std::cin >> _data[offset]; },
        }, operand);
        ++idx;
    }
}

void Compiler::Process(const Operand& operand) {
    if(_program.empty()) {
        _program.push_back(operand);
        return;
    }

    auto& prev = _program.back();
    std::visit(overloaded {
        [&](const Add& x) {
            if(auto v = std::get_if<Set>(&prev); v) {
                v->value++;
            } else {
                CheckOperandWithPreviousAndInsert<Add, Sub>(x, prev);
            }
        },
        [&](const Sub& x) {
            if(auto v = std::get_if<Set>(&prev); v) {
                v->value--;
            } else {
                CheckOperandWithPreviousAndInsert<Sub, Add>(x, prev);
            }
        },
        [&](const PtrAdd& x) { CheckOperandWithPreviousAndInsert<PtrAdd, PtrSub>(x, prev); },
        [&](const PtrSub& x) { CheckOperandWithPreviousAndInsert<PtrSub, PtrAdd>(x, prev); },
        [&](const Set& x) { _program.push_back(x); },
        [&](const WhileBegin& x) { _program.push_back(x); },
        [&](const WhileEnd& x) { _program.push_back(x); },
        [&](const Output& x) { _program.push_back(x); },
        [&](const Input& x) { _program.push_back(x); },
    }, operand);
}

uint8_t* Compiler::PrepareGeneratedCodeEntryPoint(uint8_t* ptr) {
    auto ptr_aligned = ((reinterpret_cast<size_t>(ptr) + (kPageSize - 1)) / kPageSize) * kPageSize;
    auto entry_point = reinterpret_cast<uint8_t*>(ptr_aligned);
    mprotect(entry_point, kAsmCodeSize, PROT_READ | PROT_WRITE | PROT_EXEC);
    return entry_point;
}

void Compiler::ExecuteGeneratedCode(uint8_t* entry_point, uint8_t* data) {
    __builtin___clear_cache(entry_point, entry_point + kAsmCodeSize);
    ((void(*)(uint8_t*))entry_point)(data);
}

#pragma once

#include "Executor.h"
#include "ExecutorAsm.h"

class Compiler {
public:
    Compiler() = default;

    void Process(const char* begin, const char* end) {
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
                                Process(Assign{});
                                break;
                            }

                            const auto add = std::get_if<Add>(&_program[idx + 1]);
                            if(add && (add->count == 1)) {
                                _program.pop_back();
                                _program.pop_back();
                                Process(Assign{});
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

    void ExecuteOptimized(size_t data_size = kDefaultDataSize) {
        ExecutorAsm executor(_program, data_size);
        executor.Run();
    }

    void Execute(size_t data_size = kDefaultDataSize) {
        Executor executor(_program, data_size);
        executor.Run();
    }

    const std::vector<Operand>& GetProgram() const { return _program; }

private:
    void Process(const Operand& operand) {
        if(_program.empty()) {
            _program.push_back(operand);
            return;
        }

        auto& prev = _program.back();
        std::visit(overloaded {
            [&](const Add& x) {
                if(auto v = std::get_if<Assign>(&prev); v) {
                    v->value++;
                } else {
                    CheckOperandWithPreviousAndInsert<Add, Sub>(x, prev);
                }
            },
            [&](const Sub& x) {
                if(auto v = std::get_if<Assign>(&prev); v) {
                    v->value--;
                } else {
                    CheckOperandWithPreviousAndInsert<Sub, Add>(x, prev);
                }
            },
            [&](const PtrAdd& x) { CheckOperandWithPreviousAndInsert<PtrAdd, PtrSub>(x, prev); },
            [&](const PtrSub& x) { CheckOperandWithPreviousAndInsert<PtrSub, PtrAdd>(x, prev); },
            [&](const Assign& x) { _program.push_back(x); },
            [&](const Mul& x) { throw -1; },
            [&](const WhileBegin& x) { _program.push_back(x); },
            [&](const WhileEnd& x) { _program.push_back(x); },
            [&](const Output& x) { _program.push_back(x); },
            [&](const Input& x) { _program.push_back(x); },
        }, operand);
    }

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

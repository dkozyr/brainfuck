#include "Compiler.h"
#include "Executor.h"
#include "ExecutorAsm.h"

#include <boost/iostreams/device/mapped_file.hpp>

Compiler::Compiler(const std::string& script_path) {
    boost::iostreams::mapped_file mmap(script_path, boost::iostreams::mapped_file::readonly);
    auto begin = mmap.const_data();
    auto end = begin + mmap.size();
    Process(begin, end);
}

void Compiler::ExecuteOptimized(size_t data_size) {
    ExecutorAsm executor(_program, data_size);
    executor.Run();
}

void Compiler::Execute(size_t data_size) {
    Executor executor(_program, data_size);
    executor.Run();
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

void Compiler::Process(const Operand& operand) {
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

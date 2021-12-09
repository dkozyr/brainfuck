#pragma once

#include "Common.h"

#include <variant>
#include <iostream>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Add{
    size_t count = 1;
    int32_t offset = 0;
};

struct Sub{
    size_t count = 1;
    int32_t offset = 0;
};

struct Set{
    size_t value = 0;
    int32_t offset = 0;
};

struct PtrAdd{
    size_t count = 1;
};

struct PtrSub{
    size_t count = 1;
};

struct WhileBegin{
    int32_t offset = 0;
};

struct WhileEnd{
    int32_t offset = 0;
};

struct Output{
    int32_t offset = 0;
};

struct Input{
    int32_t offset = 0;
};

using Operand = std::variant<Add, Sub, Set, PtrAdd, PtrSub, WhileBegin, WhileEnd, Output, Input>;

inline bool operator==(const Add& left, const Add& right) { return (left.count == right.count) && (left.offset == right.offset); }
inline bool operator==(const Sub& left, const Sub& right) { return (left.count == right.count) && (left.offset == right.offset); }
inline bool operator==(const Set& left, const Set& right) { return (left.value == right.value) && (left.offset == right.offset); }
inline bool operator==(const PtrAdd& left, const PtrAdd& right) { return (left.count == right.count); }
inline bool operator==(const PtrSub& left, const PtrSub& right) { return (left.count == right.count); }
inline bool operator==(const WhileBegin& left, const WhileBegin& right) { return (left.offset == right.offset); }
inline bool operator==(const WhileEnd& left, const WhileEnd& right) { return (left.offset == right.offset); }
inline bool operator==(const Output& left, const Output& right) { return (left.offset == right.offset); }
inline bool operator==(const Input& left, const Input& right) { return (left.offset == right.offset); }

inline void DebugOperand(const Operand& operand) {
    std::visit(overloaded {
        [&](const Add& x) { std::cout << "+[" << x.offset << "]" << x.count << "|"; },
        [&](const Sub& x) { std::cout << "-[" << x.offset << "]" << x.count << "|"; },
        [&](const Set& x) { std::cout << "s[" << x.offset << "]" << x.value << "|"; },
        [&](const PtrAdd& x) { std::cout << "p+" << x.count << "|"; },
        [&](const PtrSub& x) { std::cout << "p-" << x.count << "|"; },
        [&](const WhileBegin& x) { std::cout << "B[" << x.offset << "]|"; },
        [&](const WhileEnd& x) { std::cout << "E[" << x.offset << "]|"; },
        [&](const Output& x) { std::cout << ".[" << x.offset << "]" << "|"; },
        [&](const Input& x) { std::cout << "*[" << x.offset << "]" << "|"; },
    }, operand);
}

inline void DebugProgram(const std::vector<Operand>& program) {
    for(auto& x: program) {
        DebugOperand(x);
    }
    std::cout << std::endl;
}        

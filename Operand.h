#pragma once

#include <variant>
#include <iostream>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Add{ size_t count = 1; };
struct Sub{ size_t count = 1; };
struct Set{ size_t value = 0; };

struct PtrAdd{ size_t count = 1; };
struct PtrSub{ size_t count = 1; };

struct WhileBegin{};
struct WhileEnd{};

struct Output{};
struct Input{};

using Operand = std::variant<Add, Sub, Set, PtrAdd, PtrSub, WhileBegin, WhileEnd, Output, Input>;

inline void DebugOperand(const Operand& operand) {
    std::visit(overloaded {
        [&](const Add& x) { std::cout << "Add: " << x.count << std::endl; },
        [&](const Sub& x) { std::cout << "Sub: " << x.count << std::endl; },
        [&](const Set& x) { std::cout << "Set: " << x.value << std::endl; },
        [&](const PtrAdd& x) { std::cout << "PtrAdd: " << x.count << std::endl; },
        [&](const PtrSub& x) { std::cout << "PtrSub: " << x.count << std::endl; },
        [&](const WhileBegin& x) { std::cout << "WhileBegin" << std::endl; },
        [&](const WhileEnd& x) { std::cout << "WhileEnd" << std::endl; },
        [&](const Output& x) { std::cout << "Output" << std::endl; },
        [&](const Input& x) { std::cout << "Input" << std::endl; },
    }, operand);
}

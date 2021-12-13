#pragma once

#include "Operand.h"
#include "BracketMap.h"

class Executor {
public:
    Executor(const std::vector<Operand>& program, size_t data_size = kDefaultDataSize);

    void Run();

private:
    static BracketMap CreateBracketMap(const std::vector<Operand>& program);

private:
    const std::vector<Operand> _program;
    std::vector<uint8_t> _data;
};

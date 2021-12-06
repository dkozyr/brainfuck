#pragma once

#include <cstdint>

extern "C" {

#include <sys/mman.h> //need for mprotect

}

extern "C" {

size_t OperandWriterProlog(uint8_t* data);
size_t OperandWriterRet(uint8_t* ptr);

size_t OperandWriterInc(uint8_t* data);
size_t OperandWriterAdd(uint8_t* data, uint8_t value);
size_t OperandWriterDec(uint8_t* data);
size_t OperandWriterSub(uint8_t* data, uint8_t value);
size_t OperandWriterSet(uint8_t* data, uint8_t value);

size_t OperandWriterPtrInc(uint8_t* data);
size_t OperandWriterPtrAdd(uint8_t* data, uint32_t value);
size_t OperandWriterPtrDec(uint8_t* data);
size_t OperandWriterPtrSub(uint8_t* data, uint32_t value);

size_t OperandWriterWhileBegin(uint8_t* data, uint32_t jump_offset);
size_t OperandWriterWhileEnd(uint8_t* data, uint32_t jump_offset);

size_t OperandWriterOutput(uint8_t* data);
size_t OperandWriterInput(uint8_t* data); //TODO: implement

}

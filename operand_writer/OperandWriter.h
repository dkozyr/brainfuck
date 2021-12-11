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

size_t OperandWriterMul(uint8_t* data, uint8_t value, int32_t factor_offset);
size_t OperandWriterMulWithOffset(uint8_t* data, uint8_t value, int32_t factor_offset, int32_t offset);

size_t OperandWriterIncWithOffset(uint8_t* data, int32_t offset);
size_t OperandWriterAddWithOffset(uint8_t* data, uint8_t value, int32_t offset);
size_t OperandWriterDecWithOffset(uint8_t* data, int32_t offset);
size_t OperandWriterSubWithOffset(uint8_t* data, uint8_t value, int32_t offset);
size_t OperandWriterSetWithOffset(uint8_t* data, uint8_t value, int32_t offset);

size_t OperandWriterPtrInc(uint8_t* data);
size_t OperandWriterPtrAdd(uint8_t* data, uint32_t value);
size_t OperandWriterPtrDec(uint8_t* data);
size_t OperandWriterPtrSub(uint8_t* data, uint32_t value);

size_t OperandWriterWhileBegin(uint8_t* data, uint32_t jump_offset);
size_t OperandWriterWhileEnd(uint8_t* data, uint32_t jump_offset);

size_t OperandWriterWhileBeginWithOffset(uint8_t* data, int32_t offset, uint32_t jump_offset);
size_t OperandWriterWhileEndWithOffset(uint8_t* data, int32_t offset, uint32_t jump_offset);

size_t OperandWriterOutput(uint8_t* data);
size_t OperandWriterInput(uint8_t* data); //TODO: implement

size_t OperandWriterOutputWithOffset(uint8_t* data, int32_t offset);
size_t OperandWriterInputWithOffset(uint8_t* data, int32_t offset); //TODO: implement

}

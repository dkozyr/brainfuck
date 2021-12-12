global OperandWriterRet
global OperandWriterProlog

global OperandWriterInc
global OperandWriterAdd
global OperandWriterDec
global OperandWriterSub
global OperandWriterSet

global OperandWriterMul
global OperandWriterMulWithOffset

global OperandWriterIncWithOffset
global OperandWriterAddWithOffset
global OperandWriterDecWithOffset
global OperandWriterSubWithOffset
global OperandWriterSetWithOffset

global OperandWriterPtrInc
global OperandWriterPtrAdd
global OperandWriterPtrDec
global OperandWriterPtrSub

global OperandWriterWhileBegin
global OperandWriterWhileEnd

global OperandWriterWhileBeginWithOffset
global OperandWriterWhileEndWithOffset

global OperandWriterOutput
global OperandWriterInput

global OperandWriterOutputWithOffset
global OperandWriterInputWithOffset

;%rdi
;%rsi
;%rdx
;%rcx
;%r8
;%r9

section .data
op_prolog:          mov     RBX, RDI
OperandWriterProlog:
                    mov     RSI, op_prolog
                    mov     RCX, (OperandWriterProlog - op_prolog)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_ret:             ret
OperandWriterRet:   mov     RSI, op_ret
                    mov     RCX, (OperandWriterRet - op_ret)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_inc:             inc     byte [RBX]
OperandWriterInc:   mov     RSI, op_inc
                    mov     RCX, (OperandWriterInc - op_inc)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_add:             add     byte [RBX], 0xFF
OperandWriterAdd:   mov     RAX, RSI
                    mov     RSI, op_add
                    mov     [RSI + (OperandWriterAdd - op_add) - 1], AL
                    mov     RCX, (OperandWriterAdd - op_add)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_dec:             dec     byte [RBX]
OperandWriterDec:   mov     RSI, op_dec
                    mov     RCX, (OperandWriterDec - op_dec)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_sub:             sub     byte [RBX], 0xFF
OperandWriterSub:   mov     RAX, RSI
                    mov     RSI, op_sub
                    mov     [RSI + (OperandWriterSub - op_sub) - 1], AL
                    mov     RCX, (OperandWriterSub - op_sub)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_set:             mov     byte [RBX], 0xFF
OperandWriterSet:   mov     RAX, RSI
                    mov     RSI, op_set
                    mov     [RSI + (OperandWriterSet - op_set) - 1], AL
                    mov     RCX, (OperandWriterSet - op_set)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_mul:             mov     AL, 0xFF
                    mul     byte [RBX + 0x12345678]
                    add     [RBX], AL
OperandWriterMul:   mov     RAX, RSI
                    mov     RSI, op_mul
                    mov     [RSI + 1], AL
                    mov     [RSI + 2 + 2], EDX
                    mov     RCX, (OperandWriterMul - op_mul)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_mul_offset:      mov     AL, 0xFF
                    mul     byte [RBX + 0x12345678]
                    add     [RBX - 0x12345678], AL
OperandWriterMulWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_mul_offset
                    mov     [RSI + 1], AL
                    mov     [RSI + 2 + 2], EDX
                    mov     [RSI + 2 + 6 + 2], ECX
                    mov     RCX, (OperandWriterMulWithOffset - op_mul_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_inc_offset:      inc     byte [RBX - 0x12345678]
OperandWriterIncWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_inc_offset
                    mov     [RSI + (OperandWriterIncWithOffset - op_inc_offset) - 4], EAX
                    mov     RCX, (OperandWriterIncWithOffset - op_inc_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_add_offset:      add     byte [RBX - 0x12345678], 0xFF
OperandWriterAddWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_add_offset
                    mov     [RSI + (OperandWriterAddWithOffset - op_add_offset) - 1], AL
                    mov     [RSI + (OperandWriterAddWithOffset - op_add_offset) - 1 - 4], EDX
                    mov     RCX, (OperandWriterAddWithOffset - op_add_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_dec_offset:      dec     byte [RBX - 0x12345678]
OperandWriterDecWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_dec_offset
                    mov     [RSI + (OperandWriterDecWithOffset - op_dec_offset) - 4], EAX
                    mov     RCX, (OperandWriterDecWithOffset - op_dec_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_sub_offset:      sub     byte [RBX - 0x12345678], 0xFF
OperandWriterSubWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_sub_offset
                    mov     [RSI + (OperandWriterSubWithOffset - op_sub_offset) - 1], AL
                    mov     [RSI + (OperandWriterSubWithOffset - op_sub_offset) - 1 - 4], EDX
                    mov     RCX, (OperandWriterSubWithOffset - op_sub_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_set_offset:      mov     byte [RBX - 0x12345678], 0xFF
OperandWriterSetWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_set_offset
                    mov     [RSI + (OperandWriterSetWithOffset - op_set_offset) - 1], AL
                    mov     [RSI + (OperandWriterSetWithOffset - op_set_offset) - 1 - 4], EDX
                    mov     RCX, (OperandWriterSetWithOffset - op_set_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_ptr_inc:         inc     RBX
OperandWriterPtrInc:
                    mov     RSI, op_ptr_inc
                    mov     RCX, (OperandWriterPtrInc - op_ptr_inc)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_ptr_add:         add     RBX, 0xFFFF
OperandWriterPtrAdd:
                    mov     RAX, RSI
                    mov     RSI, op_ptr_add
                    mov     [RSI + (OperandWriterPtrAdd - op_ptr_add) - 4], EAX
                    mov     RCX, (OperandWriterPtrAdd - op_ptr_add)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_ptr_dec:         dec     RBX
OperandWriterPtrDec:
                    mov     RSI, op_ptr_dec
                    mov     RCX, (OperandWriterPtrDec - op_ptr_dec)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_ptr_sub:         sub     RBX, 0xFFFF
OperandWriterPtrSub:
                    mov     RAX, RSI
                    mov     RSI, op_ptr_sub
                    mov     [RSI + (OperandWriterPtrSub - op_ptr_sub) - 4], EAX
                    mov     RCX, (OperandWriterPtrSub - op_ptr_sub)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_output:          push    RBX
                    mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, 1                      ; 1 - stdout
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; max output_length
                    syscall
                    pop     RBX
OperandWriterOutput:mov     RSI, op_output
                    mov     RCX, (OperandWriterOutput - op_output)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_input:           push    RBX
                    xor     RAX, RAX                    ; 0 - sys_read
                    mov     RDI, RAX                    ; 0 - stdin
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; max output_length
                    syscall
                    pop     RBX
OperandWriterInput: mov     RSI, op_input
                    mov     RCX, (OperandWriterInput - op_input)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_output_offset:   push    RBX
                    add     RBX, 0x12345678
op_output_offset2:  mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, 1                      ; 1 - stdout
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; max output_length
                    syscall
                    pop     RBX
OperandWriterOutputWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_output_offset
                    mov     [RSI + (op_output_offset2 - op_output_offset) - 4], EAX
                    mov     RCX, (OperandWriterOutputWithOffset - op_output_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_input_offset:    push    RBX
                    add     RBX, 0x12345678
op_input_offset2:   xor     RAX, RAX                    ; 0 - sys_read
                    mov     RDI, RAX                    ; 0 - stdin
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; max output_length
                    syscall
                    pop     RBX
OperandWriterInputWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_input_offset
                    mov     [RSI + (op_input_offset2 - op_input_offset) - 4], EAX
                    mov     RCX, (OperandWriterInputWithOffset - op_input_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_while_begin:     cmp     byte [RBX], 0
                    jz      $+1234567
OperandWriterWhileBegin:
                    mov     RAX, RSI
                    mov     RSI, op_while_begin
                    mov     [RSI + (OperandWriterWhileBegin - op_while_begin) - 4], EAX
                    mov     RCX, (OperandWriterWhileBegin - op_while_begin)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_while_end:       cmp     byte [RBX], 0
                    jnz     $-1234567
OperandWriterWhileEnd:
                    mov     RAX, RSI
                    mov     RSI, op_while_end
                    mov     [RSI + (OperandWriterWhileEnd - op_while_end) - 4], EAX
                    mov     RCX, (OperandWriterWhileEnd - op_while_end)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_while_begin_offset:
                    cmp     byte [RBX + 0x12345678], 0
                    jz      $+1234567
OperandWriterWhileBeginWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_while_begin_offset
                    mov     [RSI + 2], EAX
                    mov     [RSI + (OperandWriterWhileBeginWithOffset - op_while_begin_offset) - 4], EDX
                    mov     RCX, (OperandWriterWhileBeginWithOffset - op_while_begin_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

op_while_end_offset:
                    cmp     byte [RBX + 0x12345678], 0
                    jnz     $-1234567
OperandWriterWhileEndWithOffset:
                    mov     RAX, RSI
                    mov     RSI, op_while_end_offset
                    mov     [RSI + 2], EAX
                    mov     [RSI + (OperandWriterWhileEndWithOffset - op_while_end_offset) - 4], EDX
                    mov     RCX, (OperandWriterWhileEndWithOffset - op_while_end_offset)
                    mov     RAX, RCX
                    rep     movsb
                    ret

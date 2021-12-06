global OperandWriterRet
global OperandWriterProlog

global OperandWriterInc
global OperandWriterAdd
global OperandWriterDec
global OperandWriterSub
global OperandWriterSet

global OperandWriterPtrInc
global OperandWriterPtrAdd
global OperandWriterPtrDec
global OperandWriterPtrSub

global OperandWriterWhileBegin
global OperandWriterWhileEnd

global OperandWriterOutput
global OperandWriterInput

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

global _start

section .bss
kMaxScriptLength    equ     10 * 1000 * 1000

result              resb    30 * 1000
script              resb    kMaxScriptLength
operand_size        resq    kMaxScriptLength
the_code            resq    kMaxScriptLength
output              resb    1000 * 1000

section .text
op_add:             add     byte [RBX], 0xFF
op_sub:             sub     byte [RBX], 0xFF
op_ptr_add:         add     RBX, 0xFF
op_ptr_sub:         sub     RBX, 0xFF
op_dot:             mov     AL, [RBX]
                    mov     [RDI], AL
                    inc     RDI
op_comma:           push    RBX
                    
                    mov     RDX, RDI
                    sub     RDX, output                 ; length
                    jz      _nothing_to_print
                    push    RBX

                    mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, RAX                    ; 1 - stdout
                    mov     RSI, output                 ; buffer
                    syscall

                    pop     RBX

_nothing_to_print:  xor     RAX, RAX                    ; 0 - sys_read
                    mov     RDI, RAX                    ; 0 - stdin
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; max output_length
                    syscall

                    pop     RBX
                    mov     RDI, output                 ; reset output buffer
op_open:            cmp     byte [RBX], 0
                    jz      $+1000000
op_close:           cmp     byte [RBX], 0
                    jnz     $-1000000
op_set_zero:        mov     byte [RBX], 0
op_eos:             mov     RDX, RDI
                    sub     RDX, output                 ; length
                    jz      _ret_without_print
                    mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, RAX                    ; 1 - stdout
                    mov     RSI, output                 ; buffer
                    syscall
_ret_without_print: ret

fd                  dq      0

OP_ADD_SIZE         equ     (op_sub - op_add)
OP_SUB_SIZE         equ     (op_ptr_add - op_sub)
OP_PTR_ADD_SIZE     equ     (op_ptr_sub - op_ptr_add)
OP_PTR_SUB_SIZE     equ     (op_dot - op_ptr_sub)
OP_OUTPUT_SIZE      equ     (op_comma - op_dot)
OP_INPUT_SIZE       equ     (op_open - op_comma)
OP_OPEN_SIZE        equ     (op_close - op_open)
OP_CLOSE_SIZE       equ     (op_set_zero - op_close)
OP_SET_ZERO_SIZE    equ     (op_eos - op_set_zero)
OP_EOS_SIZE         equ     (fd - op_eos)

ID_EOS              equ     0
ID_ADD              equ     1
ID_SUB              equ     2
ID_PTR_ADD          equ     3
ID_PTR_SUB          equ     4
ID_INPUT            equ     5
ID_OUTPUT           equ     6
ID_OPEN             equ     7
ID_CLOSE            equ     8
ID_SET_ZERO         equ     9

; ------------------------------------------------------------------------------
_start:             xor     RAX, RAX
                    mov     RCX, 3750                   ; 30000 / 8 = 3750
                    mov     RDI, result
                    rep     stosq
                    
                    pop     RAX
                    cmp     RAX, 2
                    jnz     _exit

                    pop     RAX
                    pop     RDI                         ; filename
                    xor     RSI, RSI                    ;
                    mov     RAX, 2                      ; sys_open
                    syscall
                    mov     [fd], RAX

                    mov     RDI, RAX                    ; file descriptor
                    mov     RSI, script                 ; buffer
                    mov     RDX, kMaxScriptLength       ; max output_length
                    xor     RAX, RAX                    ; sys_read
                    syscall

                    or      RAX, RAX
                    jz      _close_fd_and_exit

                    call    process_symbols_and_remove_useless
                    call    prepare_the_code

                    ; execute generated code -----------------------------------
                    mov     RBX, result
                    mov     RDI, output

                    push    _close_fd_and_exit
                    jmp     the_code

                    ; exit -----------------------------------------------------
_close_fd_and_exit: mov     RAX, 3                      ; sys_close
                    mov     RDI, [fd]
                    syscall

_exit:              mov     RAX, 60                     ; exit
                    xor     RDI, RDI                    ; exit code - 0
                    syscall

; ------------------------------------------------------------------------------
%macro process_script_symbol 3
                    cmp     AL, %1
                    jnz     .test_next_%2

                    add     RDX, %3
                    mov     AL, %2
                    jmp     _good_symbol
.test_next_%2:
%endmacro

%macro process_script_symbol_with_counter 3
                    cmp     AL, %1
                    jnz     .test_next_%3

                    dec     RSI
                    xor     RAX, RAX
.symbol_counter_%2: inc     RSI
                    inc     AL
                    mov     AH, [RSI + 1]
                    cmp     AH, %1
                    jz      .symbol_counter_%2

                    and     RAX, 0xFF
                    mov     [RBX + 8], RAX

                    add     RDX, %3
                    mov     AL, %2
                    jmp     _good_symbol
.test_next_%3:
%endmacro

%macro process_script_open_minus_close 2
                    cmp     AL, '['
                    jnz     .test_next_%1

                    mov     AH, [RSI + 1]
                    cmp     AH, '-'
                    jnz     .test_next_%1

                    mov     AH, [RSI + 2]
                    cmp     AH, ']'
                    jnz     .test_next_%1

                    add     RSI, 2
                    sub     RCX, 2

                    add     RDX, %2
                    mov     AL, %1
                    jmp     _good_symbol
.test_next_%1:
%endmacro

process_symbols_and_remove_useless:
                    mov     RSI, script
                    mov     RDI, RSI

                    xor     RDX, RDX                    ; cumulative size
                    mov     RBX, operand_size
                    mov     byte [RSI + RAX], 0         ; end of stream

_process_symbol:    mov     AL, [RSI]
                    or      AL, AL
                    jz      _end_of_stream

                    process_script_open_minus_close ID_SET_ZERO, OP_SET_ZERO_SIZE

                    process_script_symbol_with_counter '+', ID_ADD,     OP_ADD_SIZE
                    process_script_symbol_with_counter '-', ID_SUB,     OP_SUB_SIZE
                    process_script_symbol_with_counter '>', ID_PTR_ADD, OP_PTR_ADD_SIZE
                    process_script_symbol_with_counter '<', ID_PTR_SUB, OP_PTR_SUB_SIZE

                    process_script_symbol '.', ID_OUTPUT,  OP_OUTPUT_SIZE
                    process_script_symbol ',', ID_INPUT,   OP_INPUT_SIZE
                    process_script_symbol '[', ID_OPEN,    OP_OPEN_SIZE
                    process_script_symbol ']', ID_CLOSE,   OP_CLOSE_SIZE

                    jmp     _bad_symbol

_good_symbol:       mov     [RDI], AL
                    inc     RDI

                    mov     [RBX], RDX
                    add     RBX, 2 * 8

_bad_symbol:        inc     RSI
                    jnz     _process_symbol

_end_of_stream:     mov     byte [RDI], ID_EOS
                    ret

; ------------------------------------------------------------------------------
%macro find_offset 4
                    mov     RSI, RDX
                    push    RBX
                    push    RDX

                    mov     RDX, 1                      ; find corresponding bracket on RDX = 0
    %1_next:        %1      RBX, 2 * 8
                    %2      RSI
                    mov     AH, [RSI]
                    cmp     AH, %3
                    jnz     .skip1
                    inc     RDX
    .skip1:         cmp     AH, %4
                    jnz     .skip2
                    dec     RDX
    .skip2:         or      RDX, RDX
                    jnz     %1_next

                    mov     RCX, [RBX]
                    pop     RDX
                    pop     RBX

                    sub     RCX, [RBX]                  ; offset '[' - offset ']'
%endmacro

%macro process_script_symbol_to_code 3
                    cmp     AL, %1
                    jnz     process_next_%2
                    mov     RSI, %2
                    mov     RCX, %3
                    jmp     _copy_and_process_next_symbol
process_next_%2:
%endmacro

%macro process_script_symbol_to_code_with_variable 4
                    cmp     AL, %2
                    jnz     process_next_%2

                    mov     RCX, [RBX + 8]
                    mov     RSI, %3 + %4 - %1
%if %1 == 1
                    mov     byte [RSI], CL
%elif %1 == 4
                    mov     dword [RSI], ECX
%endif
                    mov     RSI, %3
                    mov     RCX, %4
                    jmp     _copy_and_process_next_symbol
process_next_%2:
%endmacro

prepare_the_code:   mov     RBX, operand_size - 2 * 8
                    mov     RDI, the_code
                    mov     RDX, script - 1

_next_symbol:       inc     RDX
                    add     RBX, 2 * 8
                    mov     AL, [RDX]

                    process_script_symbol_to_code_with_variable 1, ID_ADD, op_add, OP_ADD_SIZE
                    process_script_symbol_to_code_with_variable 1, ID_SUB, op_sub, OP_SUB_SIZE

                    process_script_symbol_to_code_with_variable 4, ID_PTR_ADD, op_ptr_add, OP_PTR_ADD_SIZE
                    process_script_symbol_to_code_with_variable 4, ID_PTR_SUB, op_ptr_sub, OP_PTR_SUB_SIZE

                    process_script_symbol_to_code ID_OUTPUT, op_dot,   OP_OUTPUT_SIZE
                    process_script_symbol_to_code ID_INPUT,  op_comma, OP_INPUT_SIZE

                    process_script_symbol_to_code ID_SET_ZERO, op_set_zero, OP_SET_ZERO_SIZE

                    cmp     AL, ID_OPEN
                    jnz     _check_close

                    find_offset add, inc, ID_OPEN, ID_CLOSE

                    mov     dword [op_open + OP_OPEN_SIZE - 4], ECX
                    mov     RSI, op_open
                    mov     RCX, OP_OPEN_SIZE
                    jmp     _copy_and_process_next_symbol

_check_close:       cmp     AL, ID_CLOSE
                    jnz     _check_eos

                    find_offset sub, dec, ID_CLOSE, ID_OPEN

                    mov     dword [op_close + OP_CLOSE_SIZE - 4], ECX
                    mov     RSI, op_close
                    mov     RCX, OP_CLOSE_SIZE
                    jmp     _copy_and_process_next_symbol

_check_eos:         cmp     AL, ID_EOS
                    jz      _stop_processing

_copy_and_process_next_symbol:
                    rep     movsb                       ; RDI - dest, RSI - src, RCX - count
                    jmp     _next_symbol

_stop_processing:   mov     RSI, op_eos
                    mov     RCX, OP_EOS_SIZE
                    rep     movsb
                    ret

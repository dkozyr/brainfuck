global _start

section .bss
kMaxScriptLength    equ     10 * 1000 * 1000

result              resb    30 * 1000
script              resb    kMaxScriptLength
clean_script        resb    kMaxScriptLength
operand_size        resq    kMaxScriptLength
ptr_offset          resq    kMaxScriptLength
the_code:           resq    kMaxScriptLength
output              resb    1000 * 1000

section .text
op_add:             add     byte [RBX + 0x1234], 0xFF
op_sub:             sub     byte [RBX + 0x1234], 0xFF
op_ptr_add:         add     RBX, 0x1234
op_ptr_sub:         sub     RBX, 0x1234
op_dot:             mov     AL, [RBX + 0x1234]
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
op_open:            cmp     byte [RBX + 0x1234], 0
                    jz      $+1000000
op_close:           jmp     $-1000000
op_set_value:       mov     byte [RBX + 0x1234], 0
op_eos:             mov     RDX, RDI
                    sub     RDX, output                 ; length
                    jz      _ret_without_print
                    mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, RAX                    ; 1 - stdout
                    mov     RSI, output                 ; buffer
                    syscall
_ret_without_print: ret

fd                  dq      0
input_file_name     dq      0

OP_ADD_SIZE         equ     (op_sub - op_add)
OP_SUB_SIZE         equ     (op_ptr_add - op_sub)
OP_PTR_ADD_SIZE     equ     (op_ptr_sub - op_ptr_add)
OP_PTR_SUB_SIZE     equ     (op_dot - op_ptr_sub)
OP_OUTPUT_SIZE      equ     (op_comma - op_dot)
OP_INPUT_SIZE       equ     (op_open - op_comma)
OP_OPEN_SIZE        equ     (op_close - op_open)
OP_CLOSE_SIZE       equ     (op_set_value - op_close)
OP_SET_VALUE_SIZE   equ     (op_eos - op_set_value)
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
ID_CLOSE_SHORT      equ     ID_CLOSE + 128
ID_SET_VALUE        equ     10

QWORD_SIZE          equ     8
OPERAND_INFO_SIZE   equ     3 * QWORD_SIZE

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
                    mov     [input_file_name], RDI
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

                    ;jmp     _close_fd_and_exit
                    ;jmp     _do_not_out_to_bin
    
                    ; write code to file .bin ----------------------------------
                    mov     RAX, RDI
                    sub     RAX, the_code               ; length
                    push    RAX

                    mov     RDI, [input_file_name]
    @find_fn_zero:  inc     RDI
                    mov     AL, [RDI]
                    or      AL, AL
                    jnz     @find_fn_zero
                    mov     byte [RDI + 0], '.'
                    mov     byte [RDI + 1], 'b'
                    mov     byte [RDI + 2], 'i'
                    mov     byte [RDI + 3], 'n'
                    mov     byte [RDI + 4], 0

                    mov     RDI, [input_file_name]
                    mov     RSI, 664o                   ; permissions
                    mov     RAX, 85                     ; sys_create
                    syscall
                    pop     RDX                         ; the_code length
                    push    RAX                         ; file descriptor

                    mov     RDI, RAX                    ; file descriptor
                    mov     RSI, the_code               ; raw code
                    mov     RAX, 1                      ; sys_write
                    syscall

                    mov     RAX, 3                      ; sys_close
                    pop     RDI                         ; file descriptor
                    syscall

_do_not_out_to_bin:
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
.symbol_counter_%2: inc     AL
.symbol_ignore_%2:  inc     RSI
                    mov     AH, [RSI + 1]
                    cmp     AH, 13
                    jz      .symbol_ignore_%2
                    cmp     AH, 10
                    jz      .symbol_ignore_%2
                    cmp     AH, %1
                    jz      .symbol_counter_%2

                    and     RAX, 0xFF
                    mov     [RBX + QWORD_SIZE], RAX

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

                    inc     RSI

                    xor     AL, AL
.counter_next_%1:   inc     RSI
                    mov     AH, [RSI + 1]
                    inc     AL
                    cmp     AH, '+'
                    jz      .counter_next_%1
                    sub     AL, 2
                    cmp     AH, '-'
                    jz      .counter_next_%1
                    inc     AL

                    and     RAX, 0xFF
                    mov     [RBX + QWORD_SIZE], RAX

                    add     RDX, %2
                    mov     AL, %1
                    jmp     _good_symbol
.test_next_%1:
%endmacro

process_symbols_and_remove_useless:
                    mov     RSI, script
                    mov     RDI, clean_script

                    xor     RDX, RDX                    ; cumulative size
                    xor     RCX, RCX                    ; cumulative ptr offset
                    mov     RBX, operand_size
                    mov     byte [RSI + RAX], 0         ; end of stream

_process_symbol:    mov     AL, [RSI]
                    or      AL, AL
                    jz      _end_of_stream

                    cmp     AL, '>'
                    jnz     .check_ptr_dec
                    inc     RCX
                    jmp     _bad_symbol     

.check_ptr_dec:     cmp     AL, '<'
                    jnz     .check_other
                    dec     RCX
                    jmp     _bad_symbol     

.check_other:       process_script_open_minus_close ID_SET_VALUE, OP_SET_VALUE_SIZE

                    process_script_symbol_with_counter '+', ID_ADD,     OP_ADD_SIZE
                    process_script_symbol_with_counter '-', ID_SUB,     OP_SUB_SIZE

                    process_script_symbol '.', ID_OUTPUT,  OP_OUTPUT_SIZE
                    process_script_symbol ',', ID_INPUT,   OP_INPUT_SIZE

                    cmp     AL, '['
                    jnz     .skip_push_rcx
                    push    RCX
.skip_push_rcx:

                    process_script_symbol '[', ID_OPEN,    OP_OPEN_SIZE

                    cmp     AL, ']'
                    jnz     _bad_symbol

                    push    RCX
                    push    RDI
                    push    RDX
                    push    RBX

                    mov     RDX, 1
.a1:                dec     RDI
                    sub     RBX, OPERAND_INFO_SIZE
                    mov     AL, [RDI]
                    cmp     AL, ID_CLOSE
                    jnz     .a2
                    inc     RDX
.a2:                cmp     AL, ID_OPEN
                    jnz     .a3
                    dec     RDX
.a3:                or      RDX, RDX
                    jnz     .a1

                    sub     RCX, [RBX + 2 * QWORD_SIZE]
                    mov     RAX, RCX

                    pop     RBX
                    pop     RDX
                    pop     RDI
                    pop     RCX

                    cmp     RAX, 0
                    jz      _process_close_bracket
                    jl      _process_sub_ptr_offset

_process_add_ptr_offset:
                    mov     byte [RDI], ID_PTR_ADD
                    inc     RDI

                    add     RDX, OP_PTR_ADD_SIZE
                    mov     [RBX], RDX
                    mov     [RBX + QWORD_SIZE], RAX
                    mov     [RBX + 2 * QWORD_SIZE], RCX
                    add     RBX, OPERAND_INFO_SIZE
                    jmp     _process_close_bracket

_process_sub_ptr_offset:
                    mov     byte [RDI], ID_PTR_SUB
                    inc     RDI

                    add     RDX, OP_PTR_SUB_SIZE
                    mov     [RBX], RDX
                    mov     [RBX + QWORD_SIZE], RAX
                    mov     [RBX + 2 * QWORD_SIZE], RCX
                    add     RBX, OPERAND_INFO_SIZE
                    jmp     _process_close_bracket

_process_close_bracket:
                    pop     RCX         ; stored RCX on open bracket
                    
                    mov     AL, [RSI]
                    process_script_symbol ']', ID_CLOSE,   OP_CLOSE_SIZE

                    jmp     _bad_symbol

_good_symbol:       mov     [RDI], AL
                    inc     RDI

                    mov     [RBX], RDX
                    mov     [RBX + 2 * QWORD_SIZE], RCX
                    add     RBX, OPERAND_INFO_SIZE

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
    .next_%1:       %1      RBX, OPERAND_INFO_SIZE
                    %2      RSI
                    mov     AH, [RSI]
                    and     AH, 0x3F
                    cmp     AH, %3
                    jnz     .skip1
                    inc     RDX
    .skip1:         cmp     AH, %4
                    jnz     .skip2
                    dec     RDX
    .skip2:         or      RDX, RDX
                    jnz     .next_%1

                    mov     RCX, [RBX]
                    ;mov     RAX, [RBX + QWORD_SIZE]

                    pop     RDX
                    pop     RBX

                    sub     RCX, [RBX]                  ; offset '[' - offset ']'
                    ;sub     RAX, [RBX + QWORD_SIZE]     ; pointer offset delta
%endmacro

%macro process_script_symbol_to_code 3
                    cmp     AL, %1
                    jnz     process_next_%2

                    mov     RSI, %2
                    mov     RCX, [RBX + 2 * QWORD_SIZE]
                    mov     dword [RSI + 2], ECX

                    mov     RCX, %3
                    jmp     _copy_and_process_next_symbol
process_next_%2:
%endmacro

%macro process_script_symbol_to_code_with_variable 4
                    cmp     AL, %2
                    jnz     process_next_%2

                    mov     RSI, %3 + 2
                    mov     RCX, [RBX + 2 * QWORD_SIZE]
                    mov     dword [RSI], ECX

                    mov     RCX, [RBX + QWORD_SIZE]
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

prepare_the_code:   mov     RBX, operand_size - OPERAND_INFO_SIZE
                    mov     RDI, the_code
                    mov     RDX, clean_script - 1

_next_symbol:       inc     RDX
                    add     RBX, OPERAND_INFO_SIZE
                    mov     AL, [RDX]

                    process_script_symbol_to_code_with_variable 1, ID_ADD, op_add, OP_ADD_SIZE
                    process_script_symbol_to_code_with_variable 1, ID_SUB, op_sub, OP_SUB_SIZE

                    process_script_symbol_to_code ID_OUTPUT, op_dot,   OP_OUTPUT_SIZE
                    process_script_symbol_to_code ID_INPUT,  op_comma, OP_INPUT_SIZE

                    process_script_symbol_to_code_with_variable 1, ID_SET_VALUE, op_set_value, OP_SET_VALUE_SIZE

                    cmp     AL, ID_PTR_ADD
                    jnz     _check_offset_sub

                    mov     RCX, [RBX + QWORD_SIZE]
                    mov     dword [op_ptr_add + OP_PTR_ADD_SIZE - 4], ECX
                    mov     RSI, op_ptr_add
                    mov     RCX, OP_PTR_ADD_SIZE
                    jmp     _copy_and_process_next_symbol

_check_offset_sub:
                    cmp     AL, ID_PTR_SUB
                    jnz     _check_open_bracket

                    xor     RCX, RCX
                    sub     RCX, [RBX + QWORD_SIZE]

                    mov     dword [op_ptr_sub + OP_PTR_SUB_SIZE - 4], ECX
                    mov     RSI, op_ptr_sub
                    mov     RCX, OP_PTR_SUB_SIZE
                    jmp     _copy_and_process_next_symbol

_check_open_bracket:
                    cmp     AL, ID_OPEN
                    jnz     _check_close

                    find_offset add, inc, ID_OPEN, ID_CLOSE

                    mov     RAX, [RBX + 2 * QWORD_SIZE]
                    mov     dword [op_open + 2], EAX

                    mov     dword [op_open + OP_OPEN_SIZE - 4], ECX
                    mov     RSI, op_open

        ;mov     [RSI + 2], EAX
        ;mov     [RSI + OP_OPEN_SIZE - 4], ECX

                    mov     RCX, OP_OPEN_SIZE
                    jmp     _copy_and_process_next_symbol

_check_close:       cmp     AL, ID_CLOSE
                    jz      _close_bracket
                    cmp     AL, ID_CLOSE_SHORT
                    jnz     _check_eos

_close_bracket:     find_offset sub, dec, ID_CLOSE, ID_OPEN

                    sub     RCX, OP_OPEN_SIZE
                    mov     dword [op_close + OP_CLOSE_SIZE - 4], ECX
                    mov     RSI, op_close
                    mov     RCX, OP_CLOSE_SIZE
                    jmp     _copy_and_process_next_symbol

;_check_close_short: cmp     AL, ID_CLOSE_SHORT
;                    jnz     _check_eos
;
;                    find_offset sub, dec, ID_CLOSE, ID_OPEN
;
;                    mov     dword [op_close_short + OP_CLOSE_SHORT_SIZE - 4], ECX
;                    mov     RSI, op_close_short
;                    mov     RCX, OP_CLOSE_SHORT_SIZE
;                    jmp     _copy_and_process_next_symbol

_check_eos:         cmp     AL, ID_EOS
                    jz      _stop_processing

_copy_and_process_next_symbol:
                    rep     movsb                       ; RDI - dest, RSI - src, RCX - count
                    jmp     _next_symbol

_stop_processing:   mov     RSI, op_eos
                    mov     RCX, OP_EOS_SIZE
                    rep     movsb
                    ret

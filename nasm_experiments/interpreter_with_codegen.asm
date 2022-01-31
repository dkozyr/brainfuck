global _start

section .bss
kMaxScriptLength    equ     10 * 1000 * 1000

result              resb    30 * 1000
script              resb    kMaxScriptLength
operand_size        resq    kMaxScriptLength
the_code            resq    kMaxScriptLength

section .text
op_plus:            inc     byte [RBX]
op_minus:           dec     byte [RBX]
op_great:           inc     RBX
op_less:            dec     RBX
op_dot:             push    RBX

                    mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, RAX                    ; 1 - stdout
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, RAX                    ; 1 - length
                    syscall

                    pop     RBX
op_comma:           push    RBX

                    xor     RAX, RAX                    ; 0 - sys_read
                    mov     RDI, RAX                    ; 0 - stdin
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; max output_length
                    syscall

                    pop     RBX
op_open:            cmp     byte [RBX], 0
                    jz      $+1000000
op_close:           cmp     byte [RBX], 0
                    jnz     $-1000000
op_eos:             ret

fd                  dq      0

OP_PLUS_SIZE        equ     (op_minus - op_plus)
OP_MINUS_SIZE       equ     (op_great - op_minus)
OP_GREAT_SIZE       equ     (op_less - op_great)
OP_LESS_SIZE        equ     (op_dot - op_less)
OP_DOT_SIZE         equ     (op_comma - op_dot)
OP_COMMA_SIZE       equ     (op_open - op_comma)
OP_OPEN_SIZE        equ     (op_close - op_open)
OP_CLOSE_SIZE       equ     (op_eos - op_close)
OP_EOS_SIZE         equ     (fd - op_eos)

EOS                 equ     0

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
                    xor     RSI, RSI
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

                    call    the_code

                    ; exit -----------------------------------------------------
_close_fd_and_exit: mov     RAX, 3                      ; sys_close
                    mov     RDI, [fd]
                    syscall

_exit:              mov     RAX, 60                     ; exit
                    xor     RDI, RDI                    ; exit code - 0
                    syscall

; ------------------------------------------------------------------------------
%macro process_symbol_with_size 2
                    cmp     AL, %1
                    jnz     .skip_%2
                    add     RDX, %2
                    jmp     .good_symbol
.skip_%2:
%endmacro

process_symbols_and_remove_useless:
                    mov     RSI, script
                    mov     RDI, RSI

                    xor     RDX, RDX                    ; cumulative size
                    mov     RBX, operand_size
                    mov     RCX, RAX

.process_symbol:    mov     AL, [RSI]

                    process_symbol_with_size '+', OP_PLUS_SIZE
                    process_symbol_with_size '-', OP_MINUS_SIZE
                    process_symbol_with_size '>', OP_GREAT_SIZE
                    process_symbol_with_size '<', OP_LESS_SIZE
                    process_symbol_with_size '.', OP_DOT_SIZE
                    process_symbol_with_size ',', OP_COMMA_SIZE
                    process_symbol_with_size '[', OP_OPEN_SIZE
                    process_symbol_with_size ']', OP_CLOSE_SIZE

                    jmp     .bad_symbol

.good_symbol:       mov     [RDI], AL
                    inc     RDI
                    mov     [RBX], RDX
                    add     RBX, 8
.bad_symbol:        inc     RSI
                    dec     RCX
                    jnz     .process_symbol

                    mov     byte [RDI], EOS
                    ret

; ------------------------------------------------------------------------------
%macro check_and_process_operand 3
                    cmp     AL, %1
                    jnz     .skip_%2
                    mov     RSI, %2
                    mov     RCX, %3
.skip_%2:
%endmacro

%macro find_offset 4
                    mov     RSI, RDX
                    push    RBX
                    push    RDX
                    ; find corresponding bracket
                    mov     RDX, 1
    %1_next:        %1      RBX, 8
                    %2      RSI
                    mov     AH, [RSI]
                    cmp     AH, %3                      ; bracket open or close
                    jnz     .skip1
                    inc     RDX
    .skip1:         cmp     AH, %4                      ; bracket close or open
                    jnz     .skip2
                    dec     RDX
    .skip2:         or      RDX, RDX
                    jnz     %1_next

                    mov     RCX, [RBX]
                    pop     RDX
                    pop     RBX

                    sub     RCX, [RBX]                  ; offset '[' - offset ']'
%endmacro

prepare_the_code:   mov     RBX, operand_size - 8
                    mov     RDI, the_code
                    mov     RDX, script - 1

_next_symbol:       inc     RDX
                    add     RBX, 8
                    mov     AL, [RDX]

                    check_and_process_operand '+', op_plus,  OP_PLUS_SIZE
                    check_and_process_operand '-', op_minus, OP_MINUS_SIZE
                    check_and_process_operand '>', op_great, OP_GREAT_SIZE
                    check_and_process_operand '<', op_less,  OP_LESS_SIZE
                    check_and_process_operand '.', op_dot, OP_DOT_SIZE
                    check_and_process_operand ',', op_comma, OP_COMMA_SIZE

                    cmp     AL, '['
                    jnz     _check_close

                    find_offset add, inc, '[', ']'

                    mov     dword [op_close - 4], ECX
                    mov     RSI, op_open
                    mov     RCX, OP_OPEN_SIZE

_check_close:       cmp     AL, ']'
                    jnz     _do_copy

                    find_offset sub, dec, ']', '['

                    mov     dword [op_eos - 4], ECX
                    mov     RSI, op_close
                    mov     RCX, OP_CLOSE_SIZE

_do_copy:           rep     movsb                       ; RDI - dest, RSI - src, RCX - count

                    cmp     AL, EOS
                    jnz     _next_symbol

                    mov     RSI, op_eos
                    mov     RCX, OP_EOS_SIZE
                    rep     movsb
                    ret

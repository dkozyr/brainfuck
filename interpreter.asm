global _start

section .bss
kMaxScriptLength    equ     10 * 1000 * 1000

result              resb    30 * 1000
script              resb    kMaxScriptLength

section .text
fd                  dq      0

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
                    mov     RAX, 0                      ; sys_read
                    syscall

                    or      RAX, RAX
                    jz      _close_fd_and_exit

                    ; ----------------------------------------------------------
                    mov     RBX, result
                    mov     RSI, script
                    mov     byte [RSI + RAX], 0         ; end of script

_loop_next_symbol:  mov     AL, [RSI]
                    or      AL, AL
                    jz      _close_fd_and_exit          ; exit on end of script

                    cmp     AL, '+'
                    jnz     _test_minus
                    inc     byte [RBX]
                    jmp     _dec_and_process_next

_test_minus:        cmp     AL, '-'
                    jnz     _test_great
                    dec     byte [RBX]
                    jmp     _dec_and_process_next

_test_great:        cmp     AL, '>'
                    jnz     _test_less
                    inc     RBX
                    jmp     _dec_and_process_next

_test_less:         cmp     AL, '<'
                    jnz     _test_dot
                    dec     RBX
                    jmp     _dec_and_process_next

_test_dot:          cmp     AL, '.'
                    jnz     _test_comma
                    push    RBX
                    push    RSI

                    mov     RAX, 1                      ; 1 - sys_write
                    mov     RDI, RAX                    ; 1 - stdout
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, RAX                    ; length
                    syscall

                    pop     RSI
                    pop     RBX
                    jmp     _dec_and_process_next

_test_comma:        cmp     AL, ','
                    jnz     _test_open
                    push    RBX
                    push    RSI

                    xor     RAX, RAX                    ; 0 - sys_read
                    mov     RDI, RAX                    ; 0 - stdin
                    mov     RSI, RBX                    ; buffer
                    mov     RDX, 1                      ; length
                    syscall

                    pop     RSI
                    pop     RBX
                    jmp     _dec_and_process_next

_test_open:         cmp     AL, '['
                    jnz     _test_close

                    cmp     byte [RBX], 0
                    jnz     _dec_and_process_next
                    mov     RDX, 1
    _move_forward:  inc     RSI
                    mov     AH, [RSI]
                    cmp     AH, '['
                    jnz     _forw_no_open
                    inc     RDX
    _forw_no_open:  cmp     AH, ']'
                    jnz     _forw_no_close
                    dec     RDX
    _forw_no_close: or      RDX, RDX
                    jnz     _move_forward
                    jmp     _dec_and_process_next

_test_close:        cmp     AL, ']'
                    jnz     _dec_and_process_next

                    cmp     byte [RBX], 0
                    jz      _dec_and_process_next
                    mov     RDX, 1
    _move_backward: dec     RSI
                    mov     AH, [RSI]
                    cmp     AH, ']'
                    jnz     _back_no_close
                    inc     RDX
    _back_no_close: cmp     AH, '['
                    jnz     _back_no_open
                    dec     RDX
    _back_no_open:  or      RDX, RDX
                    jnz     _move_backward

_dec_and_process_next:
                    inc     RSI
                    jmp     _loop_next_symbol

                    ; exit -----------------------------------------------------
_close_fd_and_exit: mov     RAX, 3                      ; sys_close
                    mov     RDI, [fd]
                    syscall

_exit:              mov     RAX, 60                     ; exit
                    xor     RDI, RDI                    ; exit code - 0
                    syscall

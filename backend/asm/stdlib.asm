global _Print 
global _start

section .text

;_start:
;    push 123123
;    call _Print
;
;    mov rax, 0x3c
;    mov rsi, 0x00
;    syscall

_Print:
    mov rax, [rsp + 8]
    call FnPrintSigned

    ret
; -------------------------------------------------------------------------------------------------
; | FnPrintUnsignedToBuffer
; | Args:   rax - Number
; | Assumes:    Number is unsigned
; | Returns:    Nothing
; | Destroys:   rdx, rdi, [PrintBuffer]
; -------------------------------------------------------------------------------------------------
FnPrintUnsignedToBuffer:
    push rbx

    mov rbx, 10
    lea rdi, [PrintBuffer + PrintBufferSize - 2]        ; set buffer end

.PrintDigit:
    xor rdx, rdx

    div rbx                                         ; divide by 10

    mov byte dl, [Digits + rdx]
    mov byte [rdi], dl

    dec rdi                                         ; print digit and decrement buffer

    cmp rax, 0
    jne .PrintDigit                                 ; check if there's no next digit

.return:
    pop rbx
    ret

; -------------------------------------------------------------------------------------------------
; | FnPrintSigned
; | Args:   rax - Number
; | Assumes:    Nothing
; | Returns:    rsi - next string character
; | Destroys:   rdx, rdi, r8, [PrintBuffer]
; -------------------------------------------------------------------------------------------------
FnPrintSigned:

    xor r8, r8
    test rax, rax
    jns .BufferPrintCall                        ; check if negative

    mov r8, 1
    neg rax                                     ; do rax = -rax if negative

.BufferPrintCall:
    call FnPrintUnsignedToBuffer                ; print value (unsigned)

    cmp r8, 0
    je .WriteNumber                             ; check if negative
    
    mov byte [rdi], '-'
    dec rdi                                     ; place minus if negative

.WriteNumber:
    push rsi

    lea rsi, [rdi + 1]
    mov rdx, PrintBuffer + PrintBufferSize - 1
    sub rdx, rdi
    mov byte [PrintBuffer + PrintBufferSize - 1], 0x0a

    call FnWriteSyscall

    pop rsi
    inc rsi

    ret

; -------------------------------------------------------------------------------------------------
; | FnWriteSyscall
; | Args:   rsi - string address
; |         rdx - strlen (rsi)        
; | Assumes:    Nothing
; | Returns:    rsi - next symbol, PrintedSymbols += rdx
; | Destroys:   Nothing
; -------------------------------------------------------------------------------------------------
FnWriteSyscall:
    push rax
    push rdi

    add [PrintBuffer], rdx   ; update printed symbols count

    mov rax, 0x01
    mov rdi, 1
    syscall                     ; make syscall

    add rsi, rdx                ; increment string pointer

    pop rdi
    pop rax
    
    ret

; -------------------------------------------------------------------------------------------------
; | FnStrlen
; | Args:   rsi - string address
; | Assumes:    Nothing
; | Returns:    rdx - string length to '\0' symbol
; |             rsi - next symbol position
; | Destroys:   Nothing
; -------------------------------------------------------------------------------------------------
FnStrlen:              ; TODO: could be vectorized
    
    mov rdx, 0

.CountLoop:        
    
    cmp byte [rsi+rdx], 0x0
    je .return                  ; check if '\0'

    inc rdx
    jmp .CountLoop

.return:
    ret

section .data

SavedRbp dq 0
SavedRet dq 0

PrintedSymbols dq 0
Digits db "0123456789abcdef"

PrintBufferSize equ 64
PrintBuffer times PrintBufferSize db 0

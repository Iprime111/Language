global _Print 
global _start

section .text

;_start:
;    call _Scan
;    push rax
;    call _Print
;
;    mov rax, 0x3c
;    mov rsi, 0x00
;    syscall

_Scan:
    mov rax, 0x00
    mov rdi, 0
    mov rsi, ScanBuffer
    mov rdx, ScanBufferSize
    syscall ; read syscall

    cmp rax, 1
    jbe .ReturnZero

    cmp rax, 2
    je .SingleSymbol

    lea rcx, [ScanBuffer + rax - 2]
    mov rax, 0
    mov r8,  1

.ReadDigit:
    mov rdx, 0
    mov byte dl, [rcx]
    
    cmp dl, '0'
    jb .ReturnZero  ; return zero if symbol < '0'

    cmp dl, '9'
    ja .ReturnZero  ; return zero if symbol > '9'

    sub dl, '0'
    imul rdx, r8 ; next digit = (symbol - '0') * 10 ^ number

    add rax, rdx

    cmp rcx, ScanBuffer + 1 ; check all symbols except the first
    je .CheckFirstSymbol

    imul r8, 10 ; rdx *= 10
    dec rcx      ; rcx--;
    jmp .ReadDigit

.CheckFirstSymbol:
    imul r8, 10
    dec rcx

    mov rdx, 0
    mov byte dl, [rcx]

    cmp dl, '-'
    je .ApplyNeg

    cmp dl, '0'
    jb .ReturnZero

    cmp dl, '9'
    ja .ReturnZero

    sub dl, '0'
    imul rdx, r8

    add rax, rdx
    ret

.ApplyNeg:
    neg rax
    ret

.ReturnZero:
    mov rax, 0

    ret

.SingleSymbol:
    mov rax, 0
    mov al, [ScanBuffer]

    cmp al, '0'
    jb .ReturnZero

    cmp al, '9'
    ja .ReturnZero

    sub al, '0'
    ret


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
FnStrlen:
    mov rdx, 0

.CountLoop:        
    
    cmp byte [rsi+rdx], 0x0
    je .Return                  ; check if '\0'

    inc rdx
    jmp .CountLoop

.Return:
    ret

section .data

SavedRbp dq 0
SavedRet dq 0

PrintedSymbols dq 0
Digits db "0123456789abcdef"

PrintBufferSize equ 32
PrintBuffer times PrintBufferSize db 0

ScanBufferSize equ 32
ScanBuffer times ScanBufferSize db 0

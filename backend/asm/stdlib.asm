global _Print 
global _start

section .text

;_start:
;    call _Scan
;    sub rsp, 8
;    movsd [rsp], xmm0
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

    call FnFindDot

    cmp byte [ScanBuffer], '-'
    je .Negative

    mov rdi, ScanBuffer
    call FnScanInteger

    jmp .ScanFraction

.Negative:
    mov rdi, ScanBuffer + 1
    call FnScanInteger
    neg rsi

.ScanFraction:
    cvtsi2sd xmm0, rsi

    lea rdi, [rcx + 1]

    lea rbx, [rcx + 1]
    sub rbx, ScanBuffer
    sub rax, rbx

    cmp rax, 5
    jbe .ScanCall

    mov rax, 5

.ScanCall:
    add rax, rbx

    lea rcx, [ScanBuffer + rax - 1]
    call FnScanInteger

    cvtsi2sd xmm1, rsi
    movsd xmm2, [TenThousandConstant]
    divsd xmm1, xmm2

    addsd xmm0, xmm1
    ret

.ReturnZero:
    xorps xmm0, xmm0
    ret
; -------------------------------------------------------------------------------------------------
; | FnScanInteger
; | Args:   rcx - buffer end
; |         rdi - buffer begin
; | Assumes:    Nothing
; | Returns:    rsi - scanned integer
; | Destroys:   r8
; -------------------------------------------------------------------------------------------------
FnScanInteger:
    push rax
    push rcx

    mov r8,  1
    mov rsi, 0

    dec rcx

.CheckCondition:
    cmp rcx, rdi
    jb .Return

    mov rax, 0
    mov al, [rcx]

    cmp al, '0'
    jb .ReturnZero

    cmp al, '9'
    ja .ReturnZero

    sub al, '0'

    imul rax, r8
    add rsi, rax
    
    imul r8, 10
    dec rcx
    jmp .CheckCondition

.Return:
    pop rcx
    pop rax
    ret

.ReturnZero:
    pop rcx
    pop rax
    ret

; -------------------------------------------------------------------------------------------------
; | FnFindDot
; | Args:   rax - symbols in buffer
; | Assumes:    rax > 1
; | Returns:    rcx - dot address (or last symbol if no dot)
; | Destroys:   Nothing
; -------------------------------------------------------------------------------------------------
FnFindDot:
    push rax
    mov rcx, ScanBuffer
    add rax, ScanBuffer - 1 ; last symbol address

.CheckCurrentAddress:
    cmp rcx, rax
    je .Return

    cmp byte [rcx], '.'
    je .Return

    inc rcx
    jmp .CheckCurrentAddress
    
.Return:
    pop rax
    ret

;TODO use buffers for printing
_Print:
    movsd xmm0, [rsp + 8]

    xorps xmm1, xmm1
    comisd xmm0, xmm1
    jae .PrintNumber

    mov rsi, Minus
    mov rdx, 1
    call FnWriteSyscall

.PrintNumber:
    cvttsd2si r9, xmm0
    mov rax, r9
    call FnPrintSigned

    imul r9, 10000

    mov rsi, Dot
    mov rdx, 1
    call FnWriteSyscall

    movsd xmm1, [TenThousandConstant]
    mulsd xmm0, xmm1
    cvttsd2si rax, xmm0

    sub rax, r9
    call FnPrintSigned

    mov rsi, NewLine
    mov rdx, 1
    call FnWriteSyscall
    ret
    

; -------------------------------------------------------------------------------------------------
; | FnPrintUnsignedToBuffer
; | Args:   rax - Number
; | Assumes:    Number is unsigned
; | Returns:    Nothing
; | Destroys:   rbx, rdx, rdi, [PrintBuffer]
; -------------------------------------------------------------------------------------------------
FnPrintUnsignedToBuffer:
    mov rbx, 10
    lea rdi, [PrintBuffer + PrintBufferSize - 1]        ; set buffer end

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
; | Destroys:   rbx, rdx, rdi, r8, [PrintBuffer]
; -------------------------------------------------------------------------------------------------
FnPrintSigned:

    test rax, rax
    jns .BufferPrintCall                        ; check if negative

    neg rax                                     ; do rax = -rax if negative

.BufferPrintCall:
    call FnPrintUnsignedToBuffer                ; print value (unsigned)

.WriteNumber:
    push rsi

    lea rsi, [rdi + 1]
    mov rdx, PrintBuffer + PrintBufferSize - 1
    sub rdx, rdi

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
Digits  db "0123456789abcdef"
Dot     db "."
Minus   db "-"
NewLine db 0x0a

PrintBufferSize equ 32
PrintBuffer times PrintBufferSize db 0

ScanBufferSize equ 32
ScanBuffer times ScanBufferSize db 0

TenThousandConstant dq 0x40c3880000000000

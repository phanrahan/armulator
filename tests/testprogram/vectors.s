
.globl _start
_start:
    b   reset
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang
    b   hang

reset:
    ldr sp,=0x20000
    bl notmain
    mov r0,#0xF0000000
hang: b hang
    str r0,[r0] ;@ cause an abort
    b hang

.globl GETPC
GETPC:
    mov r0,pc
    mov pc,lr

.globl GETSP
GETSP:
    mov r0,sp
    mov pc,lr


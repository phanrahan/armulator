
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
    mov r0,#0x001000
    mov r1,#1
    str r1,[r0] 

hang: b hang
    b hang


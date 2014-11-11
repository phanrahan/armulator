
    .code 32

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
    ldr r0,=gothumb
    bx r0
hang: b hang

@  ARM ABOVE
@---------------------------------------------
@  THUMB BELOW

    .code 16

    .thumb_func
gothumb:
    bl notmain
forever: b forever

.globl GETPC
.thumb_func
GETPC:
    mov r0,pc
    bx lr

.globl GETSP
.thumb_func
GETSP:
    mov r0,sp
    bx lr




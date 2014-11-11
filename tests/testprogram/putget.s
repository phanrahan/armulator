
;@-----------------------
;@-----------------------
.globl PUT32
PUT32:
    str r1,[r0]
    mov pc,lr
;@-----------------------
.globl PUT32RI
PUT32RI:
    str r2,[r0,r1]
    mov pc,lr
;@-----------------------
.globl GET32
GET32:
    ldr r0,[r0]
    mov pc,lr
;@-----------------------
.globl GET32RI
GET32RI:
    ldr r0,[r0,r1]
    mov pc,lr
;@-----------------------
.globl PUTGETRESET
PUTGETRESET:
    ldr r2,[r0]
    bic r2,r2,r1
    str r2,[r0]
    mov pc,lr
;@-----------------------
.globl PUTGETSET
PUTGETSET:
    ldr r2,[r0]
    orr r2,r2,r1
    str r2,[r0]
    mov pc,lr
;@-----------------------
.globl ASMDELAY
ASMDELAY:
    subs r0,r0,#1
    bne ASMDELAY
    mov pc,lr
;@-----------------------


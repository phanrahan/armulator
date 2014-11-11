
;@-----------------------
;@-----------------------
.globl PUT32
.thumb_func
PUT32:
    str r1,[r0]
    bx lr
;@-----------------------
.globl GET32
.thumb_func
GET32:
    ldr r0,[r0]
    bx lr
;@-----------------------


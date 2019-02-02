; Writes an arrow image
; to a 5x7 LED Matrix connected at 0x81
LIA 0x81
LIB 0x10
MOV [A],B
INC A
LIB 0x20
MOV [A],B
INC A
LIB 0x7f
MOV [A],B
INC A
LIB 0x20
MOV [A],B
INC A
LIB 0x10
MOV [A],B
;RETI

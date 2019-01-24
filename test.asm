; Test assembler program
; Adds contents of Ram addresses 0x0C to 0x0F
; and saves the result in the 'D' Register
LIA 0x0C ; Array start
LIC 0x04 ; Array length
LID 0x00 ; Result register
loop:
ADD D,[A]
INC A
DEC C
JNZ loop

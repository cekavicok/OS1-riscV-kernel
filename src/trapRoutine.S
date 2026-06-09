
#_ZN  [duzina_klase] [ime_klase] [duzina_funkcije] [ime_funkcije] E v
.extern _ZN5riscV10handleTrapEv

.align 4 #poravnavanje stvec na 4
.global _ZN5riscV11trapRoutineEv
.type _ZN5riscV11trapRoutineEv, @function
_ZN5riscV11trapRoutineEv:
    #addi sp, sp, -16
    #sd ra, 8(sp)
    #li a0, 84        # 'T'
    #call __putc
    #ld ra, 8(sp)
    #addi sp, sp, 16
    #provera ispisa :)
    addi sp, sp, -256  #32 x 8B
    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\ind, \ind*8(sp) #sd - store doubleword?
    .endr

    call _ZN5riscV10handleTrapEv

    .irp ind,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\ind, \ind*8(sp)
    .endr
    addi sp, sp, 256

    sret # spec -> pc
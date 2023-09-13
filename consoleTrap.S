.align 4
.extern _ZN5Riscv17handleConsoleTrapEv
.global _ZN5Riscv11consoleTrapEv
.type _ZN5Riscv11consoleTrapEv, @function
_ZN5Riscv11consoleTrapEv:

    addi sp,sp,-256
    .irp index, 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv17handleConsoleTrapEv

    .irp index, 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index*8(sp)
    .endr
    addi sp,sp,256

    sret
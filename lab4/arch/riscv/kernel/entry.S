    .section .text.entry
    .align 2
    .globl _traps 
_traps:
    # YOUR CODE HERE
    # -----------
    sd x2, 0(sp)
    addi sp, sp, -256
    sd x0, 240(sp)
    sd x1, 232(sp)
    sd x3, 224(sp)
    sd x4, 216(sp)
    sd x5, 208(sp)
    sd x6, 200(sp)
    sd x7, 192(sp)
    sd x8, 184(sp)
    sd x9, 176(sp)
    sd x10, 168(sp)
    sd x11, 160(sp)
    sd x12, 152(sp)
    sd x13, 144(sp)
    sd x14, 136(sp)
    sd x15, 128(sp)
    sd x16, 120(sp)
    sd x17, 112(sp)
    sd x18, 104(sp)
    sd x19, 96(sp)
    sd x20, 88(sp)
    sd x21, 80(sp)
    sd x22, 72(sp)
    sd x23, 64(sp)
    sd x24, 56(sp)
    sd x25, 48(sp)
    sd x26, 40(sp)
    sd x27, 32(sp)
    sd x28, 24(sp)
    sd x29, 16(sp)
    sd x30, 8(sp)
    sd x31, 0(sp)
    csrr t0, sepc
    sd t0, 248(sp)
        # 1. save 32 registers and sepc to stack

    # -----------

        # 2. call trap_handler
    csrr a0, scause
    csrr a1, sepc
    call trap_handler

    # -----------

        # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack
    ld t0, 248(sp)
    csrrw x0, sepc, t0
    ld x0, 240(sp)
    ld x1, 232(sp)
    ld x3, 224(sp)
    ld x4, 216(sp)
    ld x5, 208(sp)
    ld x6, 200(sp)
    ld x7, 192(sp)
    ld x8, 184(sp)
    ld x9, 176(sp)
    ld x10, 168(sp)
    ld x11, 160(sp)
    ld x12, 152(sp)
    ld x13, 144(sp)
    ld x14, 136(sp)
    ld x15, 128(sp)
    ld x16, 120(sp)
    ld x17, 112(sp)
    ld x18, 104(sp)
    ld x19, 96(sp)
    ld x20, 88(sp)
    ld x21, 80(sp)
    ld x22, 72(sp)
    ld x23, 64(sp)
    ld x24, 56(sp)
    ld x25, 48(sp)
    ld x26, 40(sp)
    ld x27, 32(sp)
    ld x28, 24(sp)
    ld x29, 16(sp)
    ld x30, 8(sp)
    ld x31, 0(sp)
    addi sp, sp, 256
    ld x2,  0(sp)

    # -----------

        # 4. return from trap
    sret

    # -----------
    
     .global __dummy
__dummy:
    la  t0, dummy
    csrrw x0, sepc, t0
    sret


     .globl __switch_to
__switch_to:
    sd ra, 40(a0)
    sd sp, 48(a0)
    sd s0, 56(a0)  
    sd s1, 64(a0)  
    sd s2, 72(a0)  
    sd s3, 80(a0)  
    sd s4, 88(a0)  
    sd s5, 96(a0)  
    sd s6, 104(a0) 
    sd s7, 112(a0) 
    sd s8, 120(a0) 
    sd s9, 128(a0) 
    sd s10, 136(a0)
    sd s11, 144(a0)
    
    ld ra, 40(a1)
    ld sp, 48(a1)
    ld s0, 56(a1)
    ld s1, 64(a1)
    ld s2, 72(a1)
    ld s3, 80(a1)
    ld s4, 88(a1)
    ld s5, 96(a1)
    ld s6, 104(a1)
    ld s7, 112(a1)
    ld s8, 120(a1)
    ld s9, 128(a1)
    ld s10, 136(a1)
    ld s11, 144(a1)   
    ret




.extern start_kernel

    .section .text.init
    .globl _start
_start:
    # ------------------

    la  t0, _traps
    csrrw x0, stvec, t0
        # set stvec = _traps

    # ------------------
    addi t0, zero, 1
    slli t0, t0, 5
    csrs sie, t0
        # set sie[STIE] = 1

    # ------------------
	
        # set first time interrupt
    call clock_set_next_event
    
    # ------------------
    addi t0, zero, 1
    slli t0, t0, 1
    csrs sstatus, t0
        # set sstatus[SIE] = 1

    # ------------------
    la sp, boot_stack_top
    j start_kernel
    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top:

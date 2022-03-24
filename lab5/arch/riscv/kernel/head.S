.extern start_kernel
.extern early_pgtbl
    .section .text.init
    .globl _start
_start:
    la sp, boot_stack_top
    call setup_vm
    call relocate
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
    #csrs sstatus, t0
        # set sstatus[SIE] = 1

    # ------------------
       
    call mm_init
    call setup_vm_final
    call task_init
    
    j start_kernel
relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
   
    ###################### 
    #   YOUR CODE HERE   #
    ######################
    li t0, 0xFFFFFFDF80000000
    add ra, ra, t0
    add sp, sp, t0

    # set satp with early_pgtbl
    
    ###################### 
    #   YOUR CODE HERE   #
    ######################
    add t0, zero, zero
    addi t0, t0, 8
    slli t0, t0, 60
    la t1, early_pgtbl
    slli t1, t1, 20
    srli t1, t1, 20
    srli t1, t1, 12
    add t0, t0, t1
    csrrw x0, satp, t0
    # flush tlb
    sfence.vma zero, zero
    ret
    
    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top:

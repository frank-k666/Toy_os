// trap.c 
#include "printk.h"
//#include "sbi.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"
extern struct task_struct* current; 
struct pt_regs{
    uint64 s[30];
    uint64 sepc;
    uint64 sstatus;
};

void sys_call(struct pt_regs *regs) {

    if(regs->s[14] == SYS_GETPID){
        regs->s[21] = current->pid;
    }else if(regs->s[14] == SYS_WRITE){
        char*buffer = regs->s[20];
        
        int a = printk("%s",buffer);
        regs->s[21] = a;
    }
    regs->s[31]+=4;
    return;
    }
    

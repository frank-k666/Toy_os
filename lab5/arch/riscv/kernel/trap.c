// trap.c 
#include "printk.h"
#include "sbi.h"
#include "proc.h"
//#include "syscall.h"
#define SYS_WRITE   64
#define SYS_GETPID  172
extern void sys_call(struct pt_regs *regs);
extern void clock_set_next_event();
struct pt_regs{
    uint64 s[33];
};
void trap_handler(unsigned long scause, unsigned long sepc, struct pt_regs *regs, unsigned long sp) {
    // 通过 `scause` 判断trap类型
    // 如果是interrupt 判断是否是timer interrupt
    // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟终端
    // `clock_set_next_event()` 见 4.5 节
    // 其他interrupt / exception 可以直接忽略
    // YOUR CODE HERE
    //printk("%llx\n",scause);
    if(scause == 0x8000000000000005){
            clock_set_next_event();
    	    do_timer();    
    }
    
    if(scause == 0x0000000000000008){
    	sys_call(regs);
        return;
    }
}

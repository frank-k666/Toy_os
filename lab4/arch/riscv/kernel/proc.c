//arch/riscv/kernel/proc.c
#include "proc.h"
extern void __dummy();

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组，所有的线程都保存在此

void dummy() {
//printk("dummy %d\n", current->counter);
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while(1) {
        if (last_counter == -1 || current->counter != last_counter) {
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            printk("[PID = %d] is running!  thread space begin at %llx\n", current->pid, current); 
        }
    }
    return;
}

void task_init() {
    // 1. 调用 kalloc() 为 idle 分配一个物理页
    // 2. 设置 state 为 TASK_RUNNING;
    // 3. 由于 idle 不参与调度 可以将其 counter / priority 设置为 0
    // 4. 设置 idle 的 pid 为 0
    // 5. 将 current 和 taks[0] 指向 idle
    idle = (struct task_struct*)kalloc();
    idle->state = TASK_RUNNING;
    idle->counter = 0;
    idle->priority = 0;
    idle->pid = 0;
    current = idle;
    task[0] = idle;
    printk("[PID = %d COUNTER = %d]\n",idle->pid, idle->counter);
    // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
    // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使用 rand() 来设置, pid 为该线程在线程数组中的下标。
    // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`, 
    // 4. 其中 `ra` 设置为 __dummy （见 4.3.2）的地址， `sp` 设置为 该线程申请的物理页的高地址

    for( int i = 1; i < NR_TASKS; i++ ){
        struct task_struct* tmp = (struct task_struct*)kalloc();
        tmp->state = TASK_RUNNING;
        tmp->counter = 0;
        tmp->priority = rand();
        tmp->pid = i;
        struct thread_struct thread_tmp;
        thread_tmp.ra = (uint64)__dummy;
        thread_tmp.sp = PGROUNDUP((uint64)tmp);
        tmp->thread = thread_tmp;
        task[i] = tmp;
        printk("[PID = %d COUNTER = %d]\n",tmp->pid, tmp->counter);
    }

    printk("...proc_init done!\n");
}
extern void __switch_to(struct task_struct* prev, struct task_struct* next);

void switch_to(struct task_struct* next) {
    if(current->pid == next->pid)return;
    //printk("%d\n", (int)current-(int)&current->thread.ra);
    printk("switch to [PID = %d COUNTER = %d]\n",next->pid, next->counter);
    struct task_struct* tmp = current;
    current = next;
    __switch_to(tmp, current);
    return;
}

void do_timer(void) {
    /* 1. 如果当前线程是 idle 线程 直接进行调度 */
    /* 2. 如果当前线程不是 idle 对当前线程的运行剩余时间减 1 
          若剩余时间任然大于0 则直接返回 否则进行调度 */
	//printk("timer %d\n", current->pid);
    if( current->pid == 0 ){
        printk("idle process is running!\n\n");
        schedule();
    }
    else{
        if(--(current->counter) > 0){
        //printk("%d\n",current->counter);
            return;//dummy();
            }
        else {
            printk("\n");
            schedule();
        }
    }
}

void schedule(void) {
    //printk("schedule\n");
    #ifdef SJF
        int id = 0;
        uint64 min = 100;
        for( int i = 1; i < NR_TASKS; i++ ){
            if( task[i]->counter > 0 && task[i]->counter < min ){
                id = i;
                min = task[i]->counter;
            }
        }
        if( id == 0 ){
            for( int i = 1; i < NR_TASKS; i++ ){
            task[i]->counter = rand();
            printk("SET [PID = %d COUNTER = %d]\n",i,task[i]->counter);
            }
            printk("\n");
            //for( int i = 1; i < NR_TASKS; i++ ) printk("%d",task[i]->counter);
            schedule();
        }else{
            switch_to(task[id]);
        }
    #endif
    #ifdef PRIORITY
        int id = 0;
        uint64 max = 0;
        for( int i = 1; i < NR_TASKS; i++ ){
            if( task[i]->counter > 0 && task[i]->priority > max ){
                id = i;
                max = task[i]->priority;
            }
        }
        if( id == 0 ){
            for( int i = 1; i < NR_TASKS; i++ ){
            task[i]->counter = task[i]->priority;
            printk("SET [PID = %d PRIORITY = %d COUNTER = %d]\n",i,task[i]->priority,task[i]->counter);
            }
            printk("\n");
            //for( int i = 1; i < NR_TASKS; i++ ) printk("%d",task[i]->counter);
            schedule();
        }else{
            switch_to(task[id]);
        }
    #endif
    return;
}



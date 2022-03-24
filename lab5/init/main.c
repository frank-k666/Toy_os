#include "printk.h"
#include "sbi.h"

extern void test();
extern void schedule(void);
int start_kernel() {
    printk("[S-MODE]");
    printk(" Hello RISC-V\n");
    schedule();
    test(); // DO NOT DELETE !!!

	return 0;
}

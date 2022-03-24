#include "defs.h"
#include "string.h"
#include "mm.h"

#include "printk.h"
extern uint64 _stext;
extern uint64 _etext;
extern uint64 _srodata;
extern uint64 _erodata;
extern uint64 _sdata;
extern uint64 _edata;

uint64 early_pgtbl[512] __attribute__((__aligned__(0x1000)));

void setup_vm(void) {
    /* 
    1. 由于是进行 1GB 的映射 这里不需要使用多级页表 
    2. 将 va 的 64bit 作为如下划分： | high bit | 9 bit | 30 bit |
        high bit 可以忽略
        中间9 bit 作为 early_pgtbl 的 index
        低 30 bit 作为 页内偏移 这里注意到 30 = 9 + 9 + 12， 即我们只使用根页表， 根页表的每个 entry 都对应 1GB 的区域。 
    3. Page Table Entry 的权限 V | R | W | X 位设置为 1
    */
    uint64  protn = 15;
    uint64 ppn = (PHY_START>>2);
    early_pgtbl[2] = ppn + protn;
    early_pgtbl[384] = ppn + protn;
}

// arch/riscv/kernel/vm.c 
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm) {
    /*
    pgtbl 为根页表的基地址
    va, pa 为需要映射的虚拟地址、物理地址
    sz 为映射的大小
    perm 为映射的读写权限

    创建多级页表的时候可以使用 kalloc() 来获取一页作为页表目录
    可以使用 V bit 来判断页表项是否存在
    */
    //printk("%llx\n",va);
    //printk("0x%llx\n",sz);
    //printk("%d\n",perm);
    uint64 page_size = 0x1000;
    uint64 plong = 511;
    uint64 psize = 512;
    uint64 vpn2 = (va >> 30) & plong;
    uint64 vpn1 = (va >> 21) & plong;
    uint64 vpn0 = (va >> 12) & plong;
    //printk("%d %d %d\n",vpn2,vpn1,vpn0);
    uint64 ppn_num0 = (sz + page_size - 1) / page_size ;
    uint64 ppn_num1 = (ppn_num0 + psize - 1) / psize;
    uint64 ppn_num2 = (ppn_num1 + psize - 1) / psize;
    //printk("%d %d %d\n",ppn_num2,ppn_num1,ppn_num0);
    uint64 *pa2, *pa1;
    uint64 i2, i1, i0;
    for( i2 = vpn2; (i2 < vpn2+ppn_num2) && (i2 < psize); i2++ ){
        if( (pgtbl[i2]&1) == 0 ){
            pa2 = (uint64*)kalloc();
            //printk("11pa2: %llx\n",(uint64)pa2- PA2VA_OFFSET);
            pgtbl[i2] = (((((uint64)pa2)- PA2VA_OFFSET) >> 12 )<< 10) | 1;
            //printk("pgtbl[%d]: %llx\n",i2, pgtbl[i2]>>10 << 12);
        }
        else{
            //printk("i2\n");
            pa2 = (((uint64)pgtbl[i2] >>10) << 12);
            //printk("pa2:%llx", pa2);
            //printk("1pgtbl[%d]: %llx\n",i2, pgtbl[i2]>>10 << 12);
            //pgtbl[i2] = (((uint64)pa2 >> 12 )<< 10) | 1;
        }
        
        for( i1 = vpn1; (i1 < vpn1+ppn_num1) && (i1 < psize); i1++ ){
            if( (pa2[i1]&1) == 0 ) { 
                pa1 = (uint64*)kalloc();
                pa2[i1] = (((((uint64)pa1) - PA2VA_OFFSET) >> 12) << 10) | 1;
                //printk("pa2[%d]: %llx\n", i1,pa2[i1]>>10 << 12);
            }
            else{
                pa1 = (((uint64)pa2[i1])>>10)<<12;
                //printk("1pa2[%d]: %llx\n", i1,pa2[i1]>>10 << 12);
            }
            //printk("%d\n",i1);
            //pa2[i1] = (((uint64)pa1 >> 12) << 10) | 1;
            //printk("%d\n",ppn_num0);
            //printk("pa1:%llx\n",pa1);
            for( i0 = vpn0; (i0 < vpn0+ppn_num0) && (i0 < psize); i0++ ){
            //printk("pa2:%llx, pa1:%llx\n",pa2,pa1);
                pa1[i0] = ((pa >> 12) << 10) | perm;
                //if( sz < 0x3000 ) printk("%d %d %d:%llx->pa2%llx:%llx->pa1%llx:%llx-> %llx\n",i2,i1,i0,pgtbl[i2],pa2,pa2[i1],pa1,pa1[i0],pa);
                pa += page_size;
            }
            ppn_num0 -= (i0-vpn0);
            vpn0 = 0;
            //printk("...\n%d %d %d ->%llx -> %llx\n",i2,i1,i0-1,pa1[i0-1],pa);
        }
        ppn_num1 -= (i1-vpn1);
        vpn1 = 0; 
    }
	return;
}
/* swapper_pg_dir: kernel pagetable 根目录， 在 setup_vm_final 进行映射。 */
unsigned long  swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));

void setup_vm_final(void) {
    memset(swapper_pg_dir, 0x0, PGSIZE);

    // No OpenSBI mapping required
    uint64* stext = (uint64*)&_stext;
    uint64* etext = (uint64*)&_etext;
    uint64* srodata = (uint64*)&_srodata;
    uint64* erodata = (uint64*)&_erodata;
    uint64* sdata = (uint64*)&_sdata;
    uint64* edata = (uint64*)&_edata;
    uint64 va = 0xffffffe000200000;
    uint64 offset = 0xffffffdf80000000;
    // mapping kernel text X|-|R|V
    int perm = 11;
    //printk("vm:%llx\n", swapper_pg_dir);
    create_mapping( swapper_pg_dir, va, 0x80200000, 0x2000, perm );
    
    // mapping kernel rodata -|-|R|V
    va = 0xffffffe000202000;
    perm = 3;
    create_mapping( swapper_pg_dir, va, 0x80202000, 0x1000, perm );
    
    // mapping other memory -|W|R|V
    va = 0xffffffe000203000;
    perm = 7;
    uint64 sz = PHY_SIZE- OPENSBI_SIZE - 0x3000;
    create_mapping(swapper_pg_dir, va, 0x80203000, sz, perm);

    // set satp with swapper_pg_dir
    
    //printk("va %llx\n", va+sz);
    
    asm volatile("add t0, zero, zero");
    asm volatile("addi t0, t0, 8");
    asm volatile("slli t0, t0, 60");
    asm volatile("la t1, swapper_pg_dir");
    asm volatile("li t2, 0xffffffdf80000000");
    asm volatile("sub t1, t1, t2");
    asm volatile("srli t1, t1, 12");
    asm volatile("add t0, t0, t1");
    asm volatile("csrrw x0, satp, t0");
    // flush TLB
    //printk("set...\n");
    asm volatile("sfence.vma zero, zero");
    //printk("set");
    return;
}





#include "print.h"
#include "sbi.h"

void puts(char *s) {
    while(*s){
    	int arg0 = *s++;
    	sbi_ecall(0x1, 0x0, arg0, 0, 0, 0, 0, 0);
    }
}

void puti(int x) {
    if( x == 0 ){
    	sbi_ecall(0x1, 0x0, '0', 0, 0, 0, 0, 0);
    	return;
    }
    if( x < 0 ){
	sbi_ecall(0x1, 0x0, '-', 0, 0, 0, 0, 0);
	x = -x;
    }
    int b[20];
    int i = 0;
    for(;x;x/=10) b[i++]=(x%10);
    i--;
    for(; i>=0; i--) sbi_ecall(0x1, 0x0, b[i]+'0', 0, 0, 0, 0, 0);
    return;
}

#include "../lib/console.h"
//clean->qemu
void main(){
    __putc('o');
    __putc('s');
    __putc('1');
    __putc('\n');
    while(1){
        char c = __getc();
        __putc(c+30);
    }




}
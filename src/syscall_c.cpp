#include "../h/syscall_c.h"
#include "../lib/console.h"

/*
__asm__ __volatile__ (
    "assembly_instructions"  // Assembly code (optional)
    : output_operands        // Outputs (modified by assembly)
    : input_operands         // Inputs (used by assembly)
    : clobber_list           // Resources modified by assembly
);
 */


void* mem_alloc (size_t size){
    size_t blocks = size/MEM_BLOCK_SIZE;
    if(size%MEM_BLOCK_SIZE != 0) blocks++;
    __asm__ volatile("mv a1, %0" : : "r" (blocks));
    __asm__ volatile("mv a0, %0" : : "r" (CODE_MEM_ALLOC));
    __asm__ volatile("ecall");

    void* addr;
    __asm__ volatile("mv %0, a0" : "=r" (addr));
    return addr;
}


int mem_free (void* ptr){
    __asm__ volatile("mv a1, %0" : : "r" (ptr));
    __asm__ volatile("mv a0, %0" : : "r" (CODE_MEM_FREE));
    __asm__ volatile("ecall");
    int status;
    __asm__ volatile("mv %0, a0" : "=r" (status));
    return status;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    void* stack = nullptr;
    if (start_routine != nullptr) stack = mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a4, %0" : : "r" (handle));
    __asm__ volatile("mv a3, %0" : : "r" (start_routine));
    __asm__ volatile("mv a2, %0" : : "r" (arg));
    __asm__ volatile("mv a1, %0" : : "r" (stack));
    __asm__ volatile("mv a0, %0" : : "r" (CODE_THREAD_CREATE));
    __asm__ volatile("ecall");
    int ret_handle;
    __asm__ volatile("mv %0, a0" : "=r" (ret_handle));
    return ret_handle;
}

int thread_exit() {
    __asm__ volatile("mv a0, %0" : : "r" (CODE_THREAD_EXIT));
    __asm__ volatile("ecall");
    int ret;
    __asm__ volatile("mv %0, a0" : "=r" (ret));
    return ret;
}

void thread_dispatch() {
    __asm__ volatile("mv a0, %0" : : "r" (CODE_THREAD_DISPATCH));
    __asm__ volatile("ecall");
}





char getc() {
    return __getc();
    /*
    __asm__ volatile("mv a0, %0" : : "r" (CODE_GETC));
    __asm__ volatile("ecall");
    char c;
    __asm__ volatile("mv %0, a0" : "=r" (c));
    return c;*/
}

void putc(char c) {
    __putc(c);
    /*
    __asm__ volatile ("mv a1, %0" : : "r" (c));
    __asm__ volatile ("mv a0, %0" : : "r" (CODE_PUTC));
    __asm__ volatile ("ecall");*/
}
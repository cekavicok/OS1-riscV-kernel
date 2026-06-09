#include "../h/syscall_c.h"


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
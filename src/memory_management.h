#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stddef.h>

void init_memory_management();
void* allocate_memory(size_t size);
void free_memory(void* ptr);

void* allocate_process_memory(int pid, size_t size);
void free_process_memory(int pid, void* ptr);

// Heap management for smaller allocations
void init_heap();
void* malloc(size_t size);
void free(void* ptr);

#endif
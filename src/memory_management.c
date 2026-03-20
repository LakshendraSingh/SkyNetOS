/*
 * Simple page-based memory manager.
 * Initializes a fixed-size memory pool, allocates one page per request,
 * and frees pages by marking them as available again.
 */

#include <stddef.h>   // for size_t
#include <stdint.h>   // for uint8_t, uintptr_t

#define PAGE_SIZE 4096
#define TOTAL_PAGES 1024

typedef struct {
    uint8_t allocated;  // 1 = allocated, 0 = free
} page_frame_t;

static page_frame_t page_table[TOTAL_PAGES];
static uint8_t memory_pool[TOTAL_PAGES * PAGE_SIZE];  // back the pages with real memory
static int last_page_index = 0;  // For Next-Fit optimization

void init_memory_management() {
    for (int i = 0; i < TOTAL_PAGES; i++) {
        page_table[i].allocated = 0;
    }
    last_page_index = 0;
}

/**
 * Allocates a single page of memory using Next-Fit strategy.
 * This improves performance by starting the search from where the last allocation ended.
 * @param size Requested size (must be <= PAGE_SIZE).
 * @return Pointer to allocated memory or NULL if no page is available.
 */
void* allocate_memory(size_t size) {
    if (size > PAGE_SIZE) {
        return NULL;
    }

    // Find a free page starting from last_page_index (Next-Fit)
    for (int i = 0; i < TOTAL_PAGES; i++) {
        int idx = (last_page_index + i) % TOTAL_PAGES;
        if (!page_table[idx].allocated) {
            page_table[idx].allocated = 1;
            last_page_index = (idx + 1) % TOTAL_PAGES; // Save hint for next time
            return (void*)&memory_pool[idx * PAGE_SIZE];
        }
    }

    return NULL;  // No free page
}

void free_memory(void* ptr) {
    uintptr_t offset = (uintptr_t)ptr - (uintptr_t)memory_pool;

    // Ensure the pointer is page-aligned and within bounds
    if (offset % PAGE_SIZE != 0) return;

    size_t page_index = offset / PAGE_SIZE;
    if (page_index < TOTAL_PAGES) {
        page_table[page_index].allocated = 0;
    }
}

#define HEAP_MAX_SIZE (1024 * 1024)
static uint8_t heap_mem[HEAP_MAX_SIZE];

typedef struct block {
    size_t size;
    int free;
    struct block *next;
} block_t;

static block_t *free_list = (block_t *)heap_mem;

void init_heap() {
    free_list->size = HEAP_MAX_SIZE - sizeof(block_t);
    free_list->free = 1;
    free_list->next = NULL;
}

void* malloc(size_t size) {
    block_t *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split block if it's significantly larger than requested
            if (curr->size > size + sizeof(block_t) + 8) {
                block_t *new_block = (block_t *)((uint8_t *)curr + sizeof(block_t) + size);
                new_block->size = curr->size - size - sizeof(block_t);
                new_block->free = 1;
                new_block->next = curr->next;
                
                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void *)((uint8_t *)curr + sizeof(block_t));
        }
        curr = curr->next;
    }
    return NULL;
}

void free(void* ptr) {
    if (!ptr) return;
    block_t *target = (block_t *)((uint8_t *)ptr - sizeof(block_t));
    target->free = 1;
    
    // Simple coalescing pass
    block_t *curr = free_list;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(block_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

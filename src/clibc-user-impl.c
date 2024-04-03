#include <user-impl.h>
#include <string.h>
#include <unistd.h>

void _putchar(char c) {
    write(1, &c, 1);
}

char _getchar() {
    char c;
    read(0, &c, 1);
    return c;
}

void _puts(const char *str) {
    write(1, str, strlen(str));
    _putchar('\n');
}


void heap_init() {
    // Nothing to do
}

void *heap_alloc(size_t *size) {
    return mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
}

void heap_dealloc(void *memptr, size_t size) {
    munmap(memptr, size);
}

size_t get_mem_used() {
    return 0;
}

size_t get_mem_total() {
    return 0;
}

size_t get_mem_free() {
    return 0;
}

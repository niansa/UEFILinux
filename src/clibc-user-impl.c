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

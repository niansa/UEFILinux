#include "getdelim.h"

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>



int main(int argc, char **argv) {
    puts("Testing write() to stdout...");
    for (unsigned it = 0; it != 12; it++) {
        write(1, "Hello world!\n"+it, 13-it);
    }

    puts("Testing bad write()...");
    write(123456, "Hi!", 3);
    perror("Error");

    puts("Testing printf()...");
    for (unsigned it = 0; it != 16; it++) {
        printf("it = %d! ", it);
    }
    printf("\n");

    puts("Testing getline() from stdin...");
    char *input_buf = NULL;
    size_t input_len;
    printf("Type something in: ");
    my_getline(&input_buf, &input_len, 0);
    input_buf[input_len-1] = '\0';
    printf("You typed in '%s'\n", input_buf);

    puts("Testing sleep() for 5 seconds...");
    sleep(5);

    puts("Testing floating point maths...");
    {int k = 1; float i,j,r,x,y=-16;while(puts(""),y++<15)for(x=0;x++<84;putchar(" .:-;!/>)|&IH%*#"[k&15]))for(i=k=r=0;j=r*r-i*i-2+x/25,i=2*r*i+y/10,j*j+i*i<11&&k++<111;r=j);}
    sleep(1);

    puts("Press return to exit.");
    while (getchar() != '\n');

    exit(0);
}

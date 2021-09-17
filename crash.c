#include <stdio.h>
#include <stdlib.h>
int main() {
    fprintf(stderr, "Blip blop. I will exit un-gracefuly and print to STDERR, too!\n");
    exit(-1);
}

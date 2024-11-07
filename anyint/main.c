#include <stdio.h>

int main(int argc, char** argv) {
    printf("%d", ~0U >> 1);
    return 0;
}

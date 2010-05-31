#include <stdio.h>

void showFoo();

/**
 * C main calls a function that uses two C++ objects that are
 * initialized statically (before main is called)
*/
int main() {
    printf("main() called\n");
    showFoo();
}

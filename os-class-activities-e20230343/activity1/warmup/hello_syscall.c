/* hello_syscall.c */
#include <unistd.h>

int main() {
    const char *message = "Hello, System Call!\n";
    write(1, message, 20);  // fd 1 = stdout, 20 = 19 characters + '\n'
    return 0;
}

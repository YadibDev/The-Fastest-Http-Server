#include <unistd.h>

int main() {
    char *args[] = {"/usr/bin/python3", "cgi-bin/script.py", NULL};  // arguments
    char *env[] = {NULL};                   // environment (empty here)

    execve("/usr/bin/python3", args, env);

    // If execve returns, it means an error occurred
    return 1;
}
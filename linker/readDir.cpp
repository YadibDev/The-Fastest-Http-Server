#include <dirent.h>
#include <iostream>

//        DIR *opendir(const char *name);
int main()
{
    DIR *stDir = opendir("/goinfre/yadib/The-Fastest-Http-Server/linker/dir");
    if (stDir == NULL)
    {
        perror("Error:");
        return 1;
    }
    dirent *ptr = readdir(stDir);
    // DT_DIR This is a directory.
    // DT_REG This is a regular file.
    return 0;
}
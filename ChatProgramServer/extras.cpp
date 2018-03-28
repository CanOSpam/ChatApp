#include <unistd.h>

int Close(int filedes)
{
    int ret;
    ret = close(filedes);
    return ret;
}

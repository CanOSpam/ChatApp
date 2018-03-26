#include <unistd.h>

#ifndef EXTRAS_H
#define EXTRAS_H

int Close(int filedes)
{
    int ret;
    ret = close(filedes);
    return ret;
}

#endif // EXTRAS_H

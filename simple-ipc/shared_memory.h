#ifndef SHARED_MEMORY_H
# define SHARED_MEMORY_H

#include <semaphore.h>

struct SharedMemory {
    sem_t parent_sem;
    sem_t child_sem;
    char message[256];
    bool exit_flag;
};

#endif
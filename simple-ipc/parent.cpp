#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "shared_memory.h"

// used command valgrind --trace-children=yes ./parent to understand SIGSEGV error from child

int main()
{
    // Create shared memory with a name so the child can access it
    int fd = shm_open("/chat_shared_mem", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        std::cerr << "shm_open failed" << std::endl;
        return 1;
    }
    
    // Set the size of the shared memory segment
    if (ftruncate(fd, sizeof(SharedMemory)) == -1) {
        std::cerr << "ftruncate failed" << std::endl;
        shm_unlink("/chat_shared_mem");
        return 1;
    }
    
    SharedMemory *shared_mem = (SharedMemory *)mmap(
        NULL,
        sizeof(SharedMemory),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0);
        
    // we use mmap to create a shared memory space for 2 processes to have access to
    if (shared_mem == MAP_FAILED)
    {
        std::cerr << "Mapping failed" << std::endl;
        shm_unlink("/chat_shared_mem");
        return 1;
    }

    // Initialize the shared memory
    memset(shared_mem, 0, sizeof(SharedMemory));
    
    // use semaphores for syncing - use named semaphores
    sem_t *parent_sem = sem_open("/parent_sem", O_CREAT, 0666, 1);
    sem_t *child_sem = sem_open("/child_sem", O_CREAT, 0666, 0);
    
    if (parent_sem == SEM_FAILED || child_sem == SEM_FAILED)
    {
        std::cerr << "Semaphore initialization failed" << std::endl;
        munmap(shared_mem, sizeof(SharedMemory)); // free shared space
        shm_unlink("/chat_shared_mem");
        return 1;
    }

    shared_mem->exit_flag = false;

    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "Fork failed" << std::endl;
        sem_close(parent_sem);
        sem_close(child_sem);
        sem_unlink("/parent_sem");
        sem_unlink("/child_sem");
        munmap(shared_mem, sizeof(SharedMemory));
        shm_unlink("/chat_shared_mem");
        return 1;
    }

    if (pid == 0)
    {
        execlp("./child", "child", NULL);

        // If exec returns, it failed, that's how it works!
        std::cerr << "Exec failed" << std::endl;
        exit(1);
    }
    else
    {
        // Parent process
        std::cout << "Parent chat program started. Type 'exit' to quit.\n" << std::endl;

        bool parent_exiting = false;

        while (!parent_exiting)
        {
            // Wait for semaphore
            sem_wait(parent_sem);

            if (shared_mem->exit_flag) {
                std::cout << "Child has exited. Terminating parent." << std::endl;
                parent_exiting = true;
                // We still need to signal child that we've seen the exit flag
                sem_post(child_sem);
                break;
            }

            // Get message from parent
            std::cout << "You as a Parent input a message: ";
            std::string message;
            std::getline(std::cin, message);

            if (message == "exit") {
                std::cout << "Parent exiting ..." << std::endl;
                shared_mem->exit_flag = true;
                parent_exiting = true;
            }

            // Copy message to shared memory
            strncpy(shared_mem->message, message.c_str(), 255);
            shared_mem->message[255] = '\0';

            // Signal child about your actions
            sem_post(child_sem);

            if (parent_exiting) break;
        }

        // Wait for child to terminate and CHECK status
        int status;
        pid_t child_pid = wait(&status);

        if (child_pid == -1)
        {
            std::cerr << "Error waiting for child process" << std::endl;
        }
        else
        {
            if (WIFEXITED(status))
            {
                std::cout << "Child process exited with status: " << WEXITSTATUS(status) << std::endl;
            }
            else if (WIFSIGNALED(status))
            {
                std::cout << "Child process killed by signal: " << WTERMSIG(status) << std::endl;
            }
            else if (WIFSTOPPED(status))
            {
                std::cout << "Child process stopped by signal: " << WSTOPSIG(status) << std::endl;
            }
        }

        // Clean up
        sem_close(parent_sem);
        sem_close(child_sem);
        sem_unlink("/parent_sem");
        sem_unlink("/child_sem");
        munmap(shared_mem, sizeof(SharedMemory));
        shm_unlink("/chat_shared_mem");
        close(fd);
    }

    return 0;
}
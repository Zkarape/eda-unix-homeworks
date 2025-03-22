#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "shared_memory.h"

int main(int argc, char* argv[]) {
    // Open the shared memory region created by the parent
    int fd = shm_open("/chat_shared_mem", O_RDWR, 0666);
    if (fd == -1) {
        std::cerr << "Failed to open shared memory" << std::endl;
        return 1;
    }
    
    // Map the shared memory segment
    SharedMemory* shared_mem = (SharedMemory*)mmap(
        NULL,
        sizeof(SharedMemory),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );
    
    if (shared_mem == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        close(fd);
        return 1;
    }
    
    // Open the semaphores created by the parent
    sem_t *parent_sem = sem_open("/parent_sem", 0);
    sem_t *child_sem = sem_open("/child_sem", 0);
    
    if (parent_sem == SEM_FAILED || child_sem == SEM_FAILED) {
        std::cerr << "Failed to open semaphores" << std::endl;
        munmap(shared_mem, sizeof(SharedMemory));
        close(fd);
        return 1;
    }
    
    std::cout << "Child chat program started. Type 'exit' to quit.\n" << std::endl;
    
    bool child_exiting = false;

    while (!child_exiting) {
        // Wait for parent's message
        sem_wait(child_sem);

        if (shared_mem->exit_flag) {
            std::cout << "Parent has set exit flag. Child exiting..." << std::endl;
            child_exiting = true;
            // Signal back to parent that we've seen the exit flag
            sem_post(parent_sem);
            break;
        }

        // Display message from parent
        std::cout << "Parent sent you a message: " << shared_mem->message << std::endl;
        
        // Get message from child
        std::cout << "You write as a child: ";
        std::string message;
        std::getline(std::cin, message);
        
        // Check for exit command
        if (message == "exit") {
            std::cout << "Child exiting..." << std::endl;
            shared_mem->exit_flag = true;
            child_exiting = true;
        }
        
        // Use shared memory finally
        strncpy(shared_mem->message, message.c_str(), 255);
        shared_mem->message[255] = '\0';
        
        // Signal parent about your action
        sem_post(parent_sem);

        if (child_exiting) break;
    }
    
    // Clean up resources
    sem_close(parent_sem);
    sem_close(child_sem);
    munmap(shared_mem, sizeof(SharedMemory));
    close(fd);
    
    return 0;
}
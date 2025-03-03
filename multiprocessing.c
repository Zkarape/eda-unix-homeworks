#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

float calculate_rect_area(float length, float width)
{
    printf("Child process: Area of rectangle: %f\n", length * width);
    return length * width;
}


float calculate_triangle_area(float base, float height)
{
    printf("Parent process: Area of triangle: %f\n", 0.5 * base * height);
    return 0.5 * base * height;
}

int main() {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        calculate_rect_area(5, 10);
        exit(0);
    }
    else {
        wait(&status);
        calculate_triangle_area(5, 10);
    }
    return 0;
}


// fork() + exec() combination is good because child process separately sets I/O redirection, while exec() executes the command in the child process. For example when we run command ls | grep "<filename>", ls and grep commands run in child processes and when detecting pipe it redirects the output of ls to grep, and exec() runs the commands. This keeps the parent process clean and atomic and doesn't need to handle I/O redirection.
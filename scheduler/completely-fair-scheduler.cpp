#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>

// The weight table is typically defined in kernel/sched/sched.h.
// The CFS logic lives in kernel/sched/fair.c.


struct Task {
    int id;
    int arrival_time;
    int burst_time;
    int weight;
    int remaining_time;
    int completion_time = -1;
    int waiting_time = 0;
    double vruntime = 0.0;
};

struct CompareTask {
    bool operator()(const Task* t1, const Task* t2) const {
        if (t1->vruntime != t2->vruntime) {
            return t1->vruntime < t2->vruntime;
        }
        return t1->id < t2->id; // lower ID first
    }
};

std::vector<Task> readTasks() {
    std::vector<Task> tasks;
    int id, arrival_time, burst_time, weight;
    while (std::cin >> id >> arrival_time >> burst_time >> weight) {
        Task task;
        task.id = id;
        task.arrival_time = arrival_time;
        task.burst_time = burst_time;
        task.weight = weight;
        task.remaining_time = burst_time;
        tasks.push_back(task);
    }
    return tasks;
}

void simulateCFS(std::vector<Task>& tasks) {
    using TaskPtr = Task*;
    using TaskTree = std::set<TaskPtr, CompareTask>;
    TaskTree runqueue;
    int current_time = 0;
    size_t next_arrival_index = 0;
    const int TIME_QUANTUM = 10; // Fixed time quantum for simplicity

    while (true) {
        // Add tasks that have arrived
        while (next_arrival_index < tasks.size() && 
               tasks[next_arrival_index].arrival_time <= current_time) {
            Task* new_task = &tasks[next_arrival_index];
            runqueue.insert(new_task);
            next_arrival_index++;
        }

        if (!runqueue.empty()) {
            // Select task with smallest vruntime
            Task* current_task = *runqueue.begin();
            runqueue.erase(runqueue.begin());

            // Execute task for a time slice
            int time_slice = std::min(TIME_QUANTUM, current_task->remaining_time);
            std::cout << "Time " << current_time << ": Task " << current_task->id 
                      << " running for " << time_slice << " units\n";
            current_task->remaining_time -= time_slice;
            current_time += time_slice;

            // Update vruntime
            current_task->vruntime += static_cast<double>(time_slice) / current_task->weight;

            if (current_task->remaining_time == 0) {
                current_task->completion_time = current_time;
                std::cout << "Task " << current_task->id << " completed at time " 
                          << current_task->completion_time << "\n";
            } else {
                runqueue.insert(current_task); // Reinsert if not finished
            }

            // Update waiting times for other tasks
            for (Task* task : runqueue) {
                task->waiting_time += time_slice;
            }
        } else {
            // Advance time if no tasks are ready
            if (next_arrival_index < tasks.size()) {
                current_time = tasks[next_arrival_index].arrival_time;
            } else {
                break; // All tasks processed
            }
        }
    }
}

void printSummary(const std::vector<Task>& tasks) {
    std::cout << "\nTask Summary:\n";
    double total_waiting_time = 0;
    double total_turnaround_time = 0;
    for (const Task& task : tasks) {
        int turnaround_time = task.completion_time - task.arrival_time;
        std::cout << "Task " << task.id << ": Completion Time = " << task.completion_time
                  << ", Waiting Time = " << task.waiting_time << ", Turnaround Time = " 
                  << turnaround_time << "\n";
        total_waiting_time += task.waiting_time;
        total_turnaround_time += turnaround_time;
    }
    std::cout << "\nAverage Waiting Time: " << total_waiting_time / tasks.size() << "\n";
    std::cout << "Average Turnaround Time: " << total_turnaround_time / tasks.size() << "\n";
}

int main() {
    std::vector<Task> tasks = readTasks();
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.arrival_time < b.arrival_time;
    });
    simulateCFS(tasks);
    printSummary(tasks);
    return 0;
}
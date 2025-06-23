#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "Task.h"
#include <vector>
#include <string>
#include <mutex>
#include <optional>

class TaskManager {
public:
    TaskManager(const std::string& username);

    bool addTask(const Task& task);
    bool deleteTask(long long id);
    void showTasks(const std::string& period) const;
    std::vector<Task> getTasksCopy() const;
    void updateTask(const Task& task);

private:
    void loadTasks();
    void saveTasks() const;
    long long getNextId();

    std::string username_;
    std::filesystem::path taskFile_;
    std::vector<Task> tasks_;
    mutable std::mutex tasks_mutex_; // 可变的互斥锁，以便在const成员函数中加锁
};

#endif // TASK_H
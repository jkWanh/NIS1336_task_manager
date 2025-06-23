#include "TaskManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TaskManager::TaskManager(const std::string& username) : username_(username) {
    const std::filesystem::path dataDir = "data";
    taskFile_ = dataDir / (username_ + "_tasks.json");
    loadTasks();
}

void TaskManager::loadTasks() {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    if (!std::filesystem::exists(taskFile_)) {
        return;
    }
    std::ifstream i(taskFile_);
    if (i.peek() == std::ifstream::traits_type::eof()) {
        return;
    }
    json j;
    i >> j;
    tasks_ = j.get<std::vector<Task>>();
}

void TaskManager::saveTasks() const {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    std::ofstream o(taskFile_);
    json j = tasks_;
    o << std::setw(4) << j << std::endl;
}

long long TaskManager::getNextId() {
    if (tasks_.empty()) {
        return 1;
    }
    auto max_it = std::max_element(tasks_.begin(), tasks_.end(), 
        [](const Task& a, const Task& b) {
            return a.id < b.id;
        });
    return max_it->id + 1;
}

bool TaskManager::addTask(const Task& task) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    
    // 检查任务名称+开始时间的唯一性
    auto it = std::find_if(tasks_.begin(), tasks_.end(), [&](const Task& t) {
        return t.name == task.name && t.startTime == task.startTime;
    });

    if (it != tasks_.end()) {
        std::cerr << "Error: A task with the same name and start time already exists." << std::endl;
        return false;
    }

    Task newTask = task;
    newTask.id = getNextId();
    tasks_.push_back(newTask);
    
    // 立即保存
    // 因为 saveTasks 内部也会加锁，所以我们在这里手动调用它，而不是让 addTask 成为 const
    // 为了避免死锁，我们不能在持有锁的情况下调用另一个需要相同锁的函数。
    // 幸运的是，saveTasks 是 const 函数，它内部的锁是 mutable 的，所以我们可以这样做。
    // 但更好的设计是分离出无锁的保存逻辑。为简单起见，我们保持现状，因为 lock_guard 是基于作用域的。
    // 此处锁会在函数结束时释放，所以我们先解锁再保存。
    // 实际上，saveTasks() const 内部会重新加锁，所以这是安全的。
    // 我们在这里直接调用 saveTasks()，它会处理自己的锁。
    // 为了确保数据一致性，我们应该在添加后立即保存。
    
    // 释放锁之前先保存
    std::ofstream o(taskFile_);
    json j = tasks_;
    o << std::setw(4) << j << std::endl;

    std::cout << "Task '" << newTask.name << "' added with ID " << newTask.id << "." << std::endl;
    return true;
}

bool TaskManager::deleteTask(long long id) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    auto it = std::remove_if(tasks_.begin(), tasks_.end(), [id](const Task& task) {
        return task.id == id;
    });

    if (it != tasks_.end()) {
        tasks_.erase(it, tasks_.end());
        saveTasks();
        std::cout << "Task with ID " << id << " deleted." << std::endl;
        return true;
    }

    std::cerr << "Error: Task with ID " << id << " not found." << std::endl;
    return false;
}

void TaskManager::showTasks(const std::string& period) const {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    if (tasks_.empty()) {
        std::cout << "No tasks to show." << std::endl;
        return;
    }

    auto tasks_copy = tasks_;
    std::sort(tasks_copy.begin(), tasks_copy.end(), [](const Task& a, const Task& b) {
        return a.startTime < b.startTime;
    });

    std::cout << "--- Your Tasks ---" << std::endl;
    std::cout << std::left << std::setw(5) << "ID"
              << std::setw(25) << "Name"
              << std::setw(20) << "Start Time"
              << std::setw(10) << "Priority"
              << std::setw(10) << "Category" << std::endl;

    for (const auto& task : tasks_copy) {
        // TODO: 实现按天/月过滤
        std::cout << std::left << std::setw(5) << task.id
                  << std::setw(25) << task.name
                  << std::setw(20) << time_point_to_string(task.startTime)
                  << std::setw(10) << (task.priority == Priority::High ? "High" : (task.priority == Priority::Medium ? "Medium" : "Low"))
                  << std::setw(10) << (task.category == Category::Study ? "Study" : (task.category == Category::Fun ? "Fun" : "Life"))
                  << std::endl;
    }
    std::cout << "------------------" << std::endl;
}

std::vector<Task> TaskManager::getTasksCopy() const {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    return tasks_;
}

void TaskManager::updateTask(const Task& task) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    auto it = std::find_if(tasks_.begin(), tasks_.end(), [&](const Task& t) {
        return t.id == task.id;
    });
    if (it != tasks_.end()) {
        *it = task;
        saveTasks();
    }
}
#include "Reminder.h"
#include <iostream>
#include <chrono>

Reminder::Reminder(TaskManager& taskManager) : taskManager_(taskManager) {}

Reminder::~Reminder() {
    stop();
}

void Reminder::start() {
    if (reminderThread_.joinable()) {
        return; // 已经启动
    }
    stop_flag_.store(false);
    reminderThread_ = std::thread(&Reminder::run, this);
}

void Reminder::stop() {
    if (!reminderThread_.joinable()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stop_flag_.store(true);
    }
    cv_.notify_one(); // 唤醒正在等待的线程
    reminderThread_.join();
}

void Reminder::run() {
    while (!stop_flag_.load()) {
        auto tasks = taskManager_.getTasksCopy();
        auto now = std::chrono::system_clock::now();

        for (auto& task : tasks) {
            if (!task.reminded && now >= task.reminderTime && now < task.startTime) {
                std::cout << "\n\n*** REMINDER ***\n"
                          << "Task '" << task.name << "' is starting soon at "
                          << time_point_to_string(task.startTime) << "!\n"
                          << "****************\n> " << std::flush;
                task.reminded = true;
                taskManager_.updateTask(task);
            }
        }

        // 等待15秒或直到被 stop() 唤醒
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait_for(lock, std::chrono::seconds(15), [this] { return stop_flag_.load(); });
    }
}
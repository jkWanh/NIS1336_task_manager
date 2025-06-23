#ifndef REMINDER_H
#define REMINDER_H

#include "TaskManager.h"
#include <thread>
#include <atomic>
#include <condition_variable>

class Reminder {
public:
    Reminder(TaskManager& taskManager);
    ~Reminder();

    void start();
    void stop();

private:
    void run();
    TaskManager& taskManager_;
    std::thread reminderThread_;
    std::atomic<bool> stop_flag_{false};
    std::condition_variable cv_;
    std::mutex mtx_;
};

#endif // REMINDER_H
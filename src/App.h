#ifndef APP_H
#define APP_H

#include "UserManager.h"
#include "TaskManager.h"
#include "Reminder.h"
#include <memory>

class App {
public:
    App();
    void runInteractive();
    
    bool login(const std::string& username, const std::string& password);
    void addTask(const std::string& name, const std::string& startTimeStr, 
                 Priority priority, Category category, int reminderMinutes);
    void deleteTask(long long id);
    void showTasks(const std::string& period);

private:
    UserManager userManager_;
    std::unique_ptr<TaskManager> taskManager_;
    std::unique_ptr<Reminder> reminder_;
};

#endif // APP_H
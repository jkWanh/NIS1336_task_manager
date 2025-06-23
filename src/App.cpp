#include "App.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <CLI/CLI.hpp>

App::App() = default;

bool App::login(const std::string& username, const std::string& password) {
    if (userManager_.login(username, password)) {
        taskManager_ = std::make_unique<TaskManager>(username);
        reminder_ = std::make_unique<Reminder>(*taskManager_);
        reminder_->start();
        std::cout << "Login successful. Welcome, " << username << "!" << std::endl;
        return true;
    }
    return false;
}

void App::addTask(const std::string& name, const std::string& startTimeStr, 
                  Priority priority, Category category, int reminderMinutes) {
    if (!taskManager_) {
        std::cerr << "Error: You must be logged in to add a task." << std::endl;
        return;
    }
    Task t;
    t.name = name;
    t.startTime = string_to_time_point(startTimeStr);
    t.priority = priority;
    t.category = category;
    t.reminderTime = t.startTime - std::chrono::minutes(reminderMinutes);
    taskManager_->addTask(t);
}

void App::deleteTask(long long id) {
    if (!taskManager_) {
        std::cerr << "Error: You must be logged in to delete a task." << std::endl;
        return;
    }
    taskManager_->deleteTask(id);
}

void App::showTasks(const std::string& period) {
    if (!taskManager_) {
        std::cerr << "Error: You must be logged in to show tasks." << std::endl;
        return;
    }
    taskManager_->showTasks(period);
}

void App::runInteractive() {
    std::string line;
    bool logged_in = false;
    std::string current_user;

    std::cout << "MySchedule Interactive Mode. Type 'help' for commands, 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (line == "exit") {
            break;
        }

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "register") {
            std::string user, pass;
            ss >> user >> pass;
            if (user.empty() || pass.empty()) {
                std::cout << "Usage: register <username> <password>" << std::endl;
            } else {
                userManager_.registerUser(user, pass);
            }
        } else if (command == "login") {
            std::string user, pass;
            ss >> user >> pass;
            if (user.empty() || pass.empty()) {
                std::cout << "Usage: login <username> <password>" << std::endl;
            } else {
                if (login(user, pass)) {
                    logged_in = true;
                    current_user = user;
                }
            }
        } else if (command == "addtask") {
            // 在交互模式下，为了简单起见，我们使用固定格式
            // 实际应用中可以使用更复杂的解析
            std::string name, start;
            std::cout << "Enter task name: ";
            std::getline(std::cin, name);
            std::cout << "Enter start time (YYYY-MM-DD HH:MM:SS): ";
            std::getline(std::cin, start);
            // 使用默认值
            addTask(name, start, Priority::Medium, Category::Life, 5);
        } else if (command == "show") {
            showTasks("all");
        } else if (command == "deltask") {
            long long id;
            ss >> id;
            deleteTask(id);
        } else if (command == "help") {
            std::cout << "Commands: register, login, addtask, show, deltask <id>, exit" << std::endl;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
}
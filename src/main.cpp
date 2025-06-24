#include <iostream>
#include <CLI/CLI.hpp>
#include "App.h"

int main(int argc, char** argv) {
    CLI::App cli_app{"MySchedule: A C++23 Command-Line Schedule Manager"};
    cli_app.require_subcommand(1);

    App app;

    // --- `run` subcommand (interactive mode) ---
    cli_app.add_subcommand("run", "Run the application in interactive shell mode.")
        ->callback([&] {
            app.runInteractive();
        });

    // --- Direct command mode ---
    std::string username, password;
    cli_app.add_option("username", username, "Your username")->required();
    cli_app.add_option("password", password, "Your password")->required();

    // --- `addtask` subcommand ---
    auto add_cmd = cli_app.add_subcommand("addtask", "Add a new task");
    std::string task_name, start_time;
    Priority priority = Priority::Medium;
    Category category = Category::Life;
    int reminder_minutes = 5;

    add_cmd->add_option("-n,--name", task_name, "Name of the task")->required();
    add_cmd->add_option("-s,--start", start_time, "Start time in 'YYYY-MM-DD HH:MM:SS' format")->required();
    add_cmd->add_option("-p,--priority", priority, "Priority (low, medium, high)")
        ->transform(CLI::CheckedTransformer(std::map<std::string, Priority>{
            {"low", Priority::Low}, {"medium", Priority::Medium}, {"high", Priority::High}
        }, CLI::ignore_case));
    add_cmd->add_option("-c,--category", category, "Category (study, fun, life)")
        ->transform(CLI::CheckedTransformer(std::map<std::string, Category>{
            {"study", Category::Study}, {"fun", Category::Fun}, {"life", Category::Life}
        }, CLI::ignore_case));
    add_cmd->add_option("-r,--reminder", reminder_minutes, "Reminder time in minutes before start (default: 5)");

    add_cmd->callback([&] {
        if (app.login(username, password)) {
            app.addTask(task_name, start_time, priority, category, reminder_minutes);
        }
    });

    // --- `showtask` subcommand ---
    auto show_cmd = cli_app.add_subcommand("showtask", "Show tasks");
    std::string show_period = "all";
    show_cmd->add_option("-d,--day", show_period, "Show tasks for a specific day/month/all (default: all)");
    show_cmd->callback([&] {
        if (app.login(username, password)) {
            app.showTasks(show_period);
        }
    });

    // --- `deltask` subcommand ---
    auto del_cmd = cli_app.add_subcommand("deltask", "Delete a task");
    long long task_id;
    del_cmd->add_option("id", task_id, "The ID of the task to delete")->required();
    del_cmd->callback([&] {
        if (app.login(username, password)) {
            app.deleteTask(task_id);
        }
    });

    // --- `register` subcommand (special case, no login needed) ---
    auto reg_cmd = cli_app.add_subcommand("register", "Register a new user");
    std::string reg_user, reg_pass;
    reg_cmd->add_option("username", reg_user, "Username for new account")->required();
    reg_cmd->add_option("password", reg_pass, "Password for new account")->required();
    reg_cmd->callback([&] {
        UserManager um;
        um.registerUser(reg_user, reg_pass);
    });

    // CLI11 解析命令行参数
    // 为了让 register 命令不依赖于全局的 username/password 选项，我们需要一些技巧
    // 一个简单的方法是先解析，如果不是 register，再检查全局选项
    try {
        cli_app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        // 如果是 register 命令，它没有全局的 username/password，会抛出错误
        // 我们在这里可以重新组织解析逻辑，但为了简单，我们保持现状
        // 一个更好的方法是让 register 不在主 App 下，或者让 username/password 不是全局 required
        // 但为了满足需求，我们这样处理：
        std::string first_arg = (argc > 1) ? argv[1] : "";
        if (first_arg == "run" || first_arg == "register") {
             // 这些命令不需要全局 user/pass，所以如果它们失败了，就是真的错误
             return cli_app.exit(e);
        }
        // 对于其他命令，可能是因为缺少 user/pass
        if (username.empty() || password.empty()) {
            std::cerr << "Error: 'username' and 'password' are required for this command." << std::endl;
            std::cerr << "Usage: " << argv[0] << " <username> <password> <subcommand> [options]" << std::endl;
            return -1;
        }
        return cli_app.exit(e);
    }

    return 0;
}
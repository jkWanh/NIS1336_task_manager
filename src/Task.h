#ifndef TASK_H
#define TASK_H

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

enum class Priority { Low, Medium, High };
enum class Category { Study, Fun, Life };

struct Task {
    long long id;
    std::string name;
    std::chrono::system_clock::time_point startTime;
    Priority priority;
    Category category;
    std::chrono::system_clock::time_point reminderTime;
    bool reminded = false; // 避免重复提醒
};

// --- JSON 序列化/反序列化 ---

// 将 std::chrono::system_clock::time_point 转换为 ISO 8601 字符串
inline std::string time_point_to_string(const std::chrono::system_clock::time_point& tp) {
    time_t t = std::chrono::system_clock::to_time_t(tp);
    char buf[sizeof("YYYY-MM-DDTHH:MM:SSZ")];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", gmtime(&t));
    return buf;
}

// 从字符串解析 time_point
inline std::chrono::system_clock::time_point string_to_time_point(const std::string& s) {
    std::tm tm = {};
    std::stringstream ss(s);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

NLOHMANN_JSON_SERIALIZE_ENUM(Priority, {
    {Priority::Low, "low"},
    {Priority::Medium, "medium"},
    {Priority::High, "high"},
})

NLOHMANN_JSON_SERIALIZE_ENUM(Category, {
    {Category::Study, "study"},
    {Category::Fun, "fun"},
    {Category::Life, "life"},
})

inline void to_json(nlohmann::json& j, const Task& t) {
    j = nlohmann::json{{"id", t.id}, {"name", t.name}, {"startTime", time_point_to_string(t.startTime)},
                         {"priority", t.priority}, {"category", t.category},
                         {"reminderTime", time_point_to_string(t.reminderTime)}, {"reminded", t.reminded}};
}

inline void from_json(const nlohmann::json& j, Task& t) {
    j.at("id").get_to(t.id);
    j.at("name").get_to(t.name);
    t.startTime = string_to_time_point(j.at("startTime").get<std::string>());
    j.at("priority").get_to(t.priority);
    j.at("category").get_to(t.category);
    t.reminderTime = string_to_time_point(j.at("reminderTime").get<std::string>());
    j.at("reminded").get_to(t.reminded);
}

#endif // TASK_H
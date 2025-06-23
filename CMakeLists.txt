cmake_minimum_required(VERSION 3.22)
project(MySchedule LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 获取依赖 ---

# 使用 FetchContent 自动下载依赖项
include(FetchContent)

# 1. CLI11 for command-line parsing
FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.4.1 # 使用一个稳定版本
)
FetchContent_MakeAvailable(CLI11)

# 2. nlohmann/json for easy JSON serialization
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# 3. libsodium for password hashing (manual find)
# 由于 brew 安装的 libsodium 不提供 CMake 配置文件，且 pkg-config 可能不存在，
# 我们直接在 brew 的默认路径下查找头文件和库文件。

# 查找头文件 (sodium.h)
find_path(SODIUM_INCLUDE_DIR sodium.h
          HINTS /opt/homebrew/include /usr/local/include)

# 查找静态库 (libsodium.a)
find_library(SODIUM_LIBRARY NAMES sodium
             HINTS /opt/homebrew/lib /usr/local/lib)

# 如果找到了头文件和库，就创建一个 IMPORTED target，这是现代CMake的最佳实践
if(SODIUM_INCLUDE_DIR AND SODIUM_LIBRARY)
    add_library(sodium::sodium STATIC IMPORTED)
    set_target_properties(sodium::sodium PROPERTIES
                          IMPORTED_LOCATION "${SODIUM_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${SODIUM_INCLUDE_DIR}")
    message(STATUS "Found libsodium library: ${SODIUM_LIBRARY}")
    message(STATUS "Found libsodium include path: ${SODIUM_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Could not find libsodium. Please install it with 'brew install libsodium'")
endif()

# --- 定义可执行文件 ---

add_executable(myschedule
    src/main.cpp
    src/App.cpp
    src/UserManager.cpp
    src/TaskManager.cpp
    src/Reminder.cpp
    src/Task.cpp
)

# 将src目录添加到myschedule目标的头文件搜索路径 (modern way)
target_include_directories(myschedule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# --- 链接库 ---

target_link_libraries(myschedule PRIVATE
    # 我们的依赖
    CLI11::CLI11
    nlohmann_json::nlohmann_json
    sodium::sodium # 使用我们手动创建的 imported target
)

# 在C++23中，std::filesystem需要链接特定库
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(myschedule PRIVATE -lstdc++fs)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(myschedule PRIVATE -lc++fs)
endif()
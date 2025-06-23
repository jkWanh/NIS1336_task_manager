# MySchedule - C++ 日程管理工具

这是一个使用 C++23 编写的命令行日程管理工具。

## 功能

- **账户管理**: 安全的用户注册和登录（密码使用Argon2加密）。
- **任务管理**: 添加、删除、显示任务。
- **数据持久化**: 任务和用户信息保存为本地JSON文件。
- **后台提醒**: 在独立线程中运行，到点提醒任务。
- **强大的命令行**: 支持交互式 `run` 模式和直接命令执行。

## 技术栈

- **语言**: C++23
- **构建**: CMake
- **依赖**:
  - `libsodium`: 用于密码哈希
  - `CLI11`: 用于命令行解析
  - `nlohmann/json`: 用于数据序列化

## 安装依赖 (macOS with Homebrew)

```bash
# 安装 C++ 编译器和 CMake
brew install cmake llvm

# 安装 libsodium
brew install libsodium
```

## 构建

```bash
mkdir build
cd build
cmake ..
make
```

## 使用方法

可执行文件名为 `myschedule`。

### 交互式模式

```bash
./myschedule run
> help
> register user1 mypassword
> login user1 mypassword
> addtask --name "学习C++" --start "2024-08-15 10:00" --priority high
> show
> exit
```

### 直接命令模式

```bash
# 添加任务
./myschedule user1 mypassword addtask --name "购物" --start "2024-08-15 18:00"

# 显示当天的任务
./myschedule user1 mypassword show
```
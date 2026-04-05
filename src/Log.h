#pragma once

#include <iostream>

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define SRC_HERE "[" __FILE_NAME__ ":" STRINGIZE(__LINE__) "]"

enum class LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR };

#define LOG(log_level, ...) log(log_level, SRC_HERE, ##__VA_ARGS__)

std::ostream &operator<<(std::ostream &out, LogLevel lvl);

void log_rec();

template <class FirstArg, class... Args>
void log_rec(FirstArg arg, Args... args) {
  std::cerr << " " << arg;
  log_rec(args...);
}

template <class... Args> void log(LogLevel lvl, const char *src, Args... args) {
  std::cerr << lvl << " " << src;
  log_rec(args...);
  std::cerr << std::endl;
}

#define ASSERT(cond, ...)                                                      \
  do {                                                                         \
    if (!(cond)) {                                                             \
      LOG(LogLevel::LOG_ERROR, "assertion failed: " #cond "\n\t",              \
          ##__VA_ARGS__);                                                      \
      __builtin_trap();                                                        \
    }                                                                          \
  } while (0)

#define TODO(...)                                                              \
  do {                                                                         \
    LOG(LogLevel::LOG_ERROR, "TODO:", ##__VA_ARGS__);                          \
    std::exit(1);                                                              \
  } while (0)

#define UNREACHABLE(...)                                                       \
  do {                                                                         \
    LOG(LogLevel::LOG_ERROR, "UNREACHABLE:", ##__VA_ARGS__);                   \
    std::exit(1);                                                              \
  } while (0)

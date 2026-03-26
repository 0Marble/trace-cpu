#pragma once

#include <iostream>

#define LOG_RED "\033[31m"
#define LOG_GREEN "\033[32m"
#define LOG_WHITE "\033[0m"

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

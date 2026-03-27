#include "Log.h"

std::ostream &operator<<(std::ostream &out, LogLevel lvl) {
  switch (lvl) {
  case LogLevel::LOG_DEBUG: {
    out << "\033[90m[DEBUG]\033[0m";
  } break;
  case LogLevel::LOG_INFO: {
    out << "\033[32m[ INFO]\033[0m";
  } break;
  case LogLevel::LOG_WARN: {
    out << "\033[31m[ WARN]\033[0m";
  } break;
  case LogLevel::LOG_ERROR: {
    out << "\033[30;101m[ERROR]\033[0m";
  } break;
  }
  return out;
}

void log_rec() {}

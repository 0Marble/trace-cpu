#include "log.h"

int main() {
  LOG(LogLevel::LOG_ERROR, "hello world", 69);
  return 0;
}

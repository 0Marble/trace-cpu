#pragma once

#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Progress {
public:
  struct Task {
    size_t id;

    bool operator==(const Task &other) const;
  };

  // these are not thread-safe
  template <class... Args>
  static Task beginGroup(size_t child_cnt, Args... args) {
    std::stringstream ss{};
    concat(ss, args...);
    return beginStr(ss.str(), child_cnt);
  }

  template <class... Args> static Task beginItem(Args... args) {
    std::stringstream ss{};
    concat(ss, args...);

    auto self = acquire();
    auto t = self->beginStrImpl(ss.str(), 0);
    self->endImpl();
    release();
    return t;
  }

  static void end() {
    acquire()->endImpl();
    release();
  }
  static void finish(Task task) {
    acquire()->finishImpl(task);
    release();
  }

  ~Progress();

private:
  struct Ctx {
    size_t operator()(const Task &t) const noexcept;
  };

  struct State {
    std::optional<Task> parent;
    std::unordered_set<Task, Ctx> children;
    std::string desc;
    size_t child_cnt;
    size_t completed;
  };

  std::vector<Task> stack = {};
  std::unordered_map<Task, State, Ctx> status = {};
  std::vector<Task> free = {};
  size_t counter = 0;

  static Task beginStr(std::string &&desc, size_t child_cnt) {
    Task t = acquire()->beginStrImpl(std::move(desc), child_cnt);
    release();
    return t;
  }

  template <class FirstArg, class... Args>
  static void concat(std::stringstream &ss, FirstArg arg, Args... args) {
    ss << arg;
    concat(ss, args...);
  }

  static void concat(std::stringstream &ss) { (void)ss; }

  static Progress *acquire();
  static void release();

  Task beginStrImpl(std::string &&desc, size_t child_cnt);
  void endImpl();
  void finishImpl(Task t);
  void print() const;
  void printRec(size_t depth, Task t) const;

  Progress();
  static Progress instance;
};

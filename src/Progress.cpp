#include "Progress.h"
#include <cstddef>
#include <functional>
#include <iostream>

#ifdef PARALLEL
#include <omp.h>
static omp_lock_t lock;
#endif

using Task = Progress::Task;
Progress::Progress() {
#ifdef PARALLEL
  omp_init_lock(&lock);
#endif
}

Progress::~Progress() {
#ifdef PARALLEL
  omp_destroy_lock(&lock);
#endif
}

Progress Progress::instance = Progress();

Task Progress::beginStrImpl(std::string &&desc, size_t child_cnt) {
  Task t = {.id = counter};

  if (free.size() != 0) {
    t = free.back();
    free.pop_back();
  } else {
    counter++;
  }

  std::optional<Task> parent = {};
  if (stack.size() != 0) {
    parent = stack.back();
    State &p = status[parent.value()];
    p.children.insert(t);
  }

  status[t] = State{
      .parent = parent,
      .children = {},
      .desc = desc,
      .child_cnt = child_cnt,
      .completed = 0,
  };
  stack.push_back(t);

  return t;
}

void Progress::endImpl() {
  stack.pop_back();
  print();
}

void Progress::finishImpl(Task t) {
  State s = status.at(t);
  (void)status.erase(t);
  if (s.parent) {
    State &p = status.at(s.parent.value());
    p.completed++;
    (void)p.children.erase(t);
  }
  free.push_back(t);

  print();
}

void Progress::printRec(size_t depth, Task t) const {
  std::string space(depth * 2, ' ');
  const State &s = status.at(t);
  std::cerr << space << "- " << s.desc << " [" << s.completed << "/"
            << s.child_cnt << "]" << "\n";

  for (auto child : s.children) {
    printRec(depth + 1, child);
  }
}

void Progress::print() const {
#ifdef NO_PROGRESS_REPORT
  return;
#endif

  std::cerr << "\033[0;0H\033[2J\n";

  for (auto &[k, v] : status) {
    if (!v.parent) {
      printRec(0, k);
    }
  }
  std::cerr << std::endl;
}

Progress *Progress::acquire() {
#ifdef PARALLEL
  omp_set_lock(&lock);
  return &instance;
#else
  return &instance;
#endif
}

void Progress::release() {
#ifdef PARALLEL
  omp_unset_lock(&lock);
#else
#endif
}

size_t Progress::Ctx::operator()(const Task &t) const noexcept {
  return std::hash<size_t>()(t.id);
}
bool Task::operator==(const Task &other) const { return id == other.id; }

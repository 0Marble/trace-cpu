#include "Object.h"

std::ostream &operator<<(std::ostream &out, const Object &obj) {
  out << obj.geometry->type();
  if (obj.debug_name) {
    out << "(" << obj.debug_name.value() << ")";
  }

  return out;
}

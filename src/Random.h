#pragma once

#include <glm/glm.hpp>

class Random {

public:
  static void init(uint64_t seed);

  static float uniform();
  static glm::vec2 uniform2();
  static glm::vec3 hemisphere();
};


#include "Random.h"
#include "glm/common.hpp"
#include "glm/gtc/random.hpp"
#include <glm/glm.hpp>

glm::vec3 Random::hemisphereUniform() {
  glm::vec3 dir = glm::sphericalRand(1.0f);
  dir.z = glm::abs(dir.z);
  return dir;
}

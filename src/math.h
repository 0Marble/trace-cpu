#pragma once

#include <glm/glm.hpp>

namespace Math {

glm::vec3 fromSpherical(glm::vec2 uv);

glm::vec2 toSpherical(glm::vec3 p);

} // namespace Math

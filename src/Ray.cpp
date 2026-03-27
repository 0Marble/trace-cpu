#include "Ray.h"
#include "glm/ext/vector_float3.hpp"

glm::vec3 Ray::at(float t) const { return t * dir + origin; }

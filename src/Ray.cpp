#include "Ray.h"
#include "Transform.h"

glm::vec3 Ray::at(float t) const { return t * dir + origin; }

template <> Ray InstantTransform::apply<Ray>(Ray &r) {
  glm::mat4 mat = asMat();

  return {
      .origin = mat * glm::vec4{r.origin.x, r.origin.y, r.origin.z, 1.0f},
      .dir = mat * glm::vec4{r.dir.x, r.dir.y, r.dir.z, 0.0f},
      .time = r.time,
  };
}

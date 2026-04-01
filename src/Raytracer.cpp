#include "Raytracer.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <array>
#include <cmath>

glm::vec3 Raytracer::traceRec(Ray ray, std::size_t bounce) {
  auto collision = scene->intersect(ray);
  if (!collision) {
    return glm::vec3(0);
  }

  auto [col, obj] = collision.value();
  InstantTransform transform = obj->transform->sample(ray.time);
  auto n2o =
      glm::mat3(col.tangent, glm::cross(col.normal, col.tangent), col.normal);
  auto w2o = transform.asInv();
  auto o2w = transform.asMat();
  auto o2n = glm::transpose(n2o);

  glm::vec3 to_view_n = o2n * (glm::vec3(w2o * glm::vec4(ray.dir, 0)));
  ray.origin = o2w * glm::vec4(col.pos, 1.0f);

  glm::vec3 direct_light = glm::vec3(0);
  for (auto &light : scene->lights) {
    glm::vec3 light_pos_w = light->worldPos(ray.time);
    ray.dir = glm::normalize(light_pos_w - ray.origin);

    if (scene->intersect(ray).has_value()) {
      continue;
    }

    auto light_color = light->sample(ray.dir, ray.time);
    if (!light_color)
      continue;

    glm::vec3 to_light_n = o2n * (w2o * glm::vec4(ray.dir, 0));

    // TODO: attenuation? same as below
    direct_light +=
        obj->material->bsdf(col.uv, to_view_n, to_light_n, ray.time) *
        light_color.value() * std::fabs(to_light_n.z);
  }

  glm::vec3 indirect_light = glm::vec3(0);
  if (bounce != bounce_cnt) {
    glm::vec3 to_obj_n = glm::reflect(to_view_n, glm::vec3(0, 0, 1));
    ray.dir = glm::normalize(o2w * glm::vec4(n2o * to_obj_n, 0.0f));

    glm::vec3 obj_light = traceRec(ray, bounce + 1);
    indirect_light +=
        obj_light * obj->material->bsdf(col.uv, to_view_n, to_obj_n, ray.time) *
        std::fabs(to_obj_n.z);
  }

  return direct_light + indirect_light;
}

glm::vec3 Raytracer::trace(Ray ray) { return traceRec(ray, 0); }

#include "Raytracer.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <array>

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

  std::size_t sources = 0;
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

    glm::vec3 light_dir = o2n * (w2o * glm::vec4(ray.dir, 0));

    // TODO: attenuation? same as below
    direct_light += obj->material->sampleColor(
        *rng, light_dir, to_view_n, light_color.value(), col.uv, ray.time);
    sources += 1;
  }
  direct_light /= (float)(sources);

  sources = 0;
  glm::vec3 indirect_light = glm::vec3(0);
  // for (std::size_t i = bounce; i < bounce_cnt; i++) {
  //   glm::vec3 outgoing =
  //       obj->material->sampleReflectedDir(*rng, to_view_n, col.uv, ray.time);
  //   ray.dir = glm::normalize(o2w * glm::vec4(n2o * outgoing, 0.0f));
  //
  //   glm::vec3 obj_light = traceRec(ray, bounce + 1);
  //   indirect_light += obj->material->sampleColor(*rng, outgoing, to_view_n,
  //                                                obj_light, col.uv,
  //                                                ray.time);
  //   sources += 1;
  // }
  // indirect_light /= (float)(sources);

  return direct_light + indirect_light;
}

glm::vec3 Raytracer::trace(Ray ray) { return traceRec(ray, 0); }

#include "Raytracer.h"
#include "Log.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <array>

Raytracer::Raytracer(std::shared_ptr<Scene> scene, size_t bounce_cnt)
    : scene(scene), bounce_cnt(bounce_cnt) {}

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

  glm::vec3 to_view_n =
      glm::normalize(o2n * (glm::vec3(w2o * glm::vec4(ray.dir, 0))));
  ray.origin = o2w * glm::vec4(col.pos, 1.0f);

  glm::vec3 direct_light = glm::vec3(0);
  size_t ray_cnt = 0;

  for (auto &light : scene->lights) {
    glm::vec3 light_pos_w = light->worldPos(ray.time);
    ray.dir = glm::normalize(light_pos_w - ray.origin);

    if (scene->intersect(ray).has_value()) {
      continue;
    }

    auto light_color = light->sample(ray.dir, ray.time);
    if (!light_color)
      continue;

    glm::vec3 to_light_n = glm::normalize(o2n * (w2o * glm::vec4(ray.dir, 0)));

    direct_light +=
        obj->material->bsdf(col.uv, ray.time, to_view_n, to_light_n) *
        light_color.value() * std::max(to_light_n.z, 0.0f) /
        glm::dot(light_pos_w - ray.origin, light_pos_w - ray.origin);
  }

  glm::vec3 indirect_light = glm::vec3(0);
  for (size_t i = bounce; i < bounce_cnt; i++) {
    float pdf = 0.0f;
    glm::vec3 bsdf = glm::vec3(0.0f);
    glm::vec3 to_obj_n =
        obj->material->sample(col.uv, ray.time, to_view_n, pdf, bsdf);
    ray.dir = glm::normalize(o2w * glm::vec4(n2o * to_obj_n, 0.0f));

    glm::vec3 obj_light = traceRec(ray, bounce + 1);
    indirect_light += obj_light * bsdf / pdf * std::max(to_obj_n.z, 0.0f);
    ray_cnt += 1;
  }

  if (ray_cnt == 0) {
    return direct_light;
  }

  return direct_light + (indirect_light) / (float)ray_cnt;
}

glm::vec3 Raytracer::trace(Ray ray) { return traceRec(ray, 0); }

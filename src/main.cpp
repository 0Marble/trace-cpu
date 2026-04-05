#include "Camera.h"
#include "DiffuseMaterial.h"
#include "Log.h"
#include "PointLight.h"
#include "Raytracer.h"
#include "Scene.h"
#include "Sphere.h"
#include "Transform.h"
#include "Triangle.h"
#include "VecFmt.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/constants.hpp"
#include <cstddef>
#include <memory>
#include <vector>

#ifdef PARALLEL
#include <omp.h>
#endif

static glm::vec3 quad[] = {
    glm::vec3(-1, -1, 0),
    glm::vec3(1, -1, 0),
    glm::vec3(1, 1, 0),
    glm::vec3(-1, 1, 0),
};

static int inds[] = {0, 1, 2, 0, 2, 3};

KeyframeTransform rotateAround(glm::vec3 center, glm::vec3 axis,
                               glm::vec3 start, float loop_cnt, float duration,
                               size_t sample_cnt = 100) {
  std::vector<InstantTransform> keyframes(sample_cnt, InstantTransform());

  float dt = glm::two_pi<float>() * loop_cnt / (float)(sample_cnt - 1);

  for (size_t i = 0; i < sample_cnt; i++) {
    float t = dt * (float)(i);
    glm::mat4 rot = glm::rotate(glm::mat4(1.0), t, axis);
    glm::vec3 pos = rot * glm::vec4(start - center, 1.0);
    pos += center;
    keyframes[i] = InstantTransform::lookAt(pos, center, axis);
    LOG(LogLevel::LOG_DEBUG, VecFmt(pos));
  }

  return KeyframeTransform(keyframes, duration);
}

int main() {
#ifdef PARALLEL
  LOG(LogLevel::LOG_INFO, "running on", omp_get_num_procs(), "omp threads");
#else
  LOG(LogLevel::LOG_INFO, "running single-threaded");
#endif

  auto scene = std::make_shared<Scene>();

  auto green = std::make_shared<DiffuseMaterial>(glm::vec3(0.1, 1.0, 0.1));
  auto red = std::make_shared<DiffuseMaterial>(glm::vec3(1.0, 0.5, 0.5));
  auto blue = std::make_shared<DiffuseMaterial>(glm::vec3(0.1, 0.1, 1.0));
  auto white = std::make_shared<DiffuseMaterial>(glm::vec3(1));

  glm::quat q_ident = {1, 0, 0, 0};
  auto back = std::make_shared<InstantTransform>(glm::vec3(0, 0, -1));
  auto bot = std::make_shared<InstantTransform>(
      glm::vec3(0, -1, 0), glm::vec3(1),
      glm::rotate(q_ident, glm::half_pi<float>(), glm::vec3(1, 0, 0)));
  auto top = std::make_shared<InstantTransform>(
      glm::vec3(0, 1, 0), glm::vec3(1),
      glm::rotate(q_ident, -glm::half_pi<float>(), glm::vec3(1, 0, 0)));
  auto sphere =
      std::make_shared<InstantTransform>(glm::vec3(0, 0, 0), glm::vec3(0.5));

  std::vector<std::shared_ptr<Triangle>> tris = {};
  for (size_t i = 0; i < sizeof(inds) / sizeof(inds[0]); i += 3) {
    auto tri = std::make_shared<Triangle>(quad[inds[i]], quad[inds[i + 1]],
                                          quad[inds[i + 2]]);
    tris.push_back(std::move(tri));
  }

  for (auto &tri : tris) {
    scene->addObject({
        .geometry = tri,
        .material = blue,
        .transform = bot,
        .debug_name = "blue",
    });
    scene->addObject({
        .geometry = tri,
        .material = green,
        .transform = back,
        .debug_name = "green",
    });
    scene->addObject({
        .geometry = tri,
        .material = red,
        .transform = top,
        .debug_name = "red",
    });
  }

  scene->addObject({
      .geometry = std::make_shared<Sphere>(),
      .material = white,
      .transform = sphere,
  });

  float light_power = 20.0f;
  scene->addLight(std::make_shared<PointLight>(light_power * glm::vec3(1, 1, 1),
                                               glm::vec3(7, 0, 0)));
  scene->addLight(std::make_shared<PointLight>(light_power * glm::vec3(1, 1, 1),
                                               glm::vec3(0, 0, -7)));
  scene->addLight(std::make_shared<PointLight>(light_power * glm::vec3(1, 1, 1),
                                               glm::vec3(-7, 0, 0)));
  scene->addLight(std::make_shared<PointLight>(light_power * glm::vec3(1, 1, 1),
                                               glm::vec3(0, 0, 7)));

  auto rt = std::make_shared<Raytracer>(scene, 3);

  auto cam_path = std::make_shared<OrbitTransform>(
      glm::vec3(0), glm::vec3(0, 0, 3), glm::vec3(0, 1, 0), 1.0f);
  // auto cam_path =
  // std::make_shared<InstantTransform>(InstantTransform::lookAt(
  //     glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

  Camera cam = Camera(cam_path, std::make_shared<UniformPixelSampler>(10),
                      Camera::Projection{}, 300, 300);

  // LOG(LogLevel::LOG_DEBUG, VecFmt(cam.pixel(rt, 0, 64, 65)));
  cam.snap(rt).save("image.png");
  // cam.record(rt, "out", 0, 1, 30);

  return 0;
}


#ifdef PARALLEL
#include <omp.h>
#endif

#include "Camera.h"
#include "DiffuseMaterial.h"
#include "Log.h"
#include "PointLight.h"
#include "Raytracer.h"
#include "Scene.h"
#include "Sphere.h"
#include "Transform.h"
#include "Triangle.h"
#include <cstddef>
#include <memory>

static glm::vec3 quad[] = {
    glm::vec3(-1, -1, 0),
    glm::vec3(1, -1, 0),
    glm::vec3(1, 1, 0),
    glm::vec3(-1, 1, 0),
};

static int inds[] = {0, 1, 2, 0, 2, 3};

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

  auto back = std::make_shared<InstantTransform>(glm::vec3(0, 0, -4));
  auto bot = std::make_shared<InstantTransform>(
      glm::vec3(0, -1, -3), glm::vec3(1),
      glm::quat(0.7071068, -0.7071068, 0, 0));
  auto left =
      std::make_shared<InstantTransform>(glm::vec3(-1, 0, -3), glm::vec3(1),
                                         glm::quat(0.7071068, 0, 0.7071068, 0));
  auto sphere =
      std::make_shared<InstantTransform>(glm::vec3(0, 0, -3), glm::vec3(0.5));

  std::vector<std::shared_ptr<Triangle>> tris = {};
  for (size_t i = 0; i < sizeof(inds) / sizeof(inds[0]); i += 3) {
    auto tri = std::make_shared<Triangle>(quad[inds[i]], quad[inds[i + 1]],
                                          quad[inds[i + 2]]);
    tris.push_back(std::move(tri));
  }

  for (auto &tri : tris) {
    scene->addObject({.geometry = tri, .material = blue, .transform = bot});
    scene->addObject({.geometry = tri, .material = green, .transform = back});
    scene->addObject({.geometry = tri, .material = red, .transform = left});
  }

  scene->addObject({
      .geometry = std::make_shared<Sphere>(),
      .material = white,
      .transform = sphere,
  });

  scene->addLight(
      std::make_shared<PointLight>(glm::vec3(1, 1, 1), glm::vec3(1.4, 0, -3)));

  scene->addLight(
      std::make_shared<PointLight>(glm::vec3(1, 1, 1), glm::vec3(10, 0, -3)));

  auto rt = std::make_shared<Raytracer>(scene, 3);

  Camera cam = Camera(
      std::make_shared<KeyframeTransform>(
          std::vector<InstantTransform>{
              InstantTransform::lookAt(glm::vec3(-1, 0, 0), glm::vec3(0, 0, -3),
                                       glm::vec3(0, 1, 0)),
              InstantTransform::lookAt(glm::vec3(1, 0, 0), glm::vec3(0, 0, -3),
                                       glm::vec3(0, 1, 0)),
          },
          1.0f),
      std::make_shared<SimplePixelSampler>(10), Camera::Projection{}, 1000,
      1000);

  float duration = 1.0f;
  size_t fps = 30;

  size_t frame_cnt = std::ceil(duration * (float)fps);
  float frame_dur = 1.0f / (float)fps;
  for (size_t i = 0; i < frame_cnt; i++) {
    float start = (float)i * frame_dur;
    float end = (float)(i + 1) * frame_dur;
    Frame f = cam.shoot(rt, start, end);
    f.save("out/frame-" + std::to_string(i) + ".png");
  }

  return 0;
}

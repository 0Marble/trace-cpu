#include "Camera.h"
#include "CpuTracer.h"
#include "Log.h"
#include "Mesh.h"
#include "PointLight.h"
#include "Random.h"
#include "Scene.h"
#include "Transform.h"
#include "VecFmt.h"
#include <memory>
#include <string>

#ifdef PARALLEL
#include <omp.h>
#endif

int main() {
#ifdef PARALLEL
  LOG(LogLevel::LOG_INFO, "running on", omp_get_max_threads(), "omp threads");
#else
  LOG(LogLevel::LOG_INFO, "running single-threaded");
#endif

  Random::init(69);

  auto cam_path = std::make_shared<OrbitTransform>(
      glm::vec3(0.0f), glm::vec3(0, 0, 3), glm::vec3(0, 1, 0), 1.0f);
  // auto cam_path =
  // std::make_shared<InstantTransform>(InstantTransform::lookAt(
  //     glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(0, 1, 0)));
  Camera cam(cam_path);

  Scene s(cam);

  Object suz = {
      .mesh = std::make_shared<Mesh>("models/Cube.obj", "Cube"),
      .material = s.default_material,
      .transform = std::make_shared<InstantTransform>(),
  };
  s.addObject(suz);
  float light_power = 20.0f;
  s.addLight(std::make_shared<PointLight>(light_power * glm::vec3(1),
                                          glm::vec3(0, 0, 10)));
  s.addLight(std::make_shared<PointLight>(light_power * glm::vec3(1),
                                          glm::vec3(10, 0, 0)));
  s.addLight(std::make_shared<PointLight>(light_power * glm::vec3(1),
                                          glm::vec3(-10, 0, 0)));
  s.addLight(std::make_shared<PointLight>(light_power * glm::vec3(1),
                                          glm::vec3(0, 0, -10)));

  CpuTracer rt(3, 100);
  auto frames = rt.record(s, 1000, 1000, 0, 1);
  for (size_t i = 0; i < frames.size(); i++) {
    frames[i].save("out/frame-" + std::to_string(i) + ".png");
  }
  // rt.snap(s, 1000, 1000, 0, 0.1).save("image.png");
  // LOG(LogLevel::LOG_DEBUG, VecFmt(rt.sampleUv(s, {0, 0}, 0)));

  return 0;
}

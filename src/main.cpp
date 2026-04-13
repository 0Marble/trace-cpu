#include "Camera.h"
#include "CpuTracer.h"
#include "Log.h"
#include "Mesh.h"
#include "Random.h"
#include "Scene.h"
#include "Transform.h"
#include <memory>

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
  Camera cam(cam_path);

  Scene s(cam);

  Object suz = {
      .mesh =
          std::make_shared<Mesh>("models/monkey_no_material.obj", "Suzanne"),
      .material = s.default_material,
      .transform = std::make_shared<InstantTransform>(),
  };
  s.addObject(suz);

  CpuTracer rt(3, 10);
  rt.snap(s, 300, 300).save("image.png");

  return 0;
}

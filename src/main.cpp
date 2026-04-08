#include "Camera.h"
#include "DiffuseMaterial.h"
#include "Log.h"
#include "ObjModel.h"
#include "PointLight.h"
#include "Random.h"
#include "Raytracer.h"
#include "Scene.h"
#include "Sphere.h"
#include "Transform.h"
#include "Triangle.h"
#include "VecFmt.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/quaternion.hpp"
#include <cstddef>
#include <memory>
#include <vector>

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

  auto scene = std::make_shared<Scene>();

  auto green = std::make_shared<DiffuseMaterial>(glm::vec3(0.1, 1.0, 0.1));
  auto red = std::make_shared<DiffuseMaterial>(glm::vec3(1.0, 0.5, 0.5));
  auto blue = std::make_shared<DiffuseMaterial>(glm::vec3(0.1, 0.1, 1.0));

  glm::quat quat_ident = {1, 0, 0, 0};
  auto model = std::make_shared<InstantTransform>(
      glm::vec3(0), glm::vec3(1),
      glm::rotate(quat_ident, glm::two_pi<float>(), glm::vec3(0, 1, 0)));

  auto monkey = std::make_shared<ObjModel>("models/monkey_no_material.obj");
  scene->addObject(ObjModel::toObject(monkey, "Suzanne", model));

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
  //     glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(0, 1, 0)));

  Camera cam = Camera(cam_path, std::make_shared<UniformPixelSampler>(4),
                      Camera::Projection{}, 1000, 1000);

  // LOG(LogLevel::LOG_DEBUG, VecFmt(cam.pixel(rt, 0, 64, 65)));
  // cam.snap(rt).save("image.png");
  cam.record(rt, "out", 0, 1, 30);

  return 0;
}

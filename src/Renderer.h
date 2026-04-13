
#pragma once

#include "Scene.h"
#include "glm/ext/vector_float3.hpp"
#include <filesystem>
#include <vector>

struct Frame {
  size_t width;
  size_t height;
  float start_time, end_time;
  std::vector<glm::vec3> pixels;

  void save(std::filesystem::path path) const;
};

class Renderer {
public:
  virtual Frame snap(Scene &scene, size_t width, size_t height,
                     float start_time = 0, float end_time = 0) = 0;

  std::vector<Frame> record(Scene &scene, size_t width, size_t height,
                            float start_time, float end_time,
                            float fps = 30.0f);
};

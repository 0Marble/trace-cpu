#pragma once

#include "Renderer.h"
#include "glm/ext/vector_float3.hpp"
#include <vector>

class CpuTracer : public Renderer {
  static const size_t tile_size = 32;
  size_t bounce_cnt;
  size_t sample_cnt;

public:
  CpuTracer(size_t bounce_cnt, size_t sample_cnt);

  Frame snap(Scene &scene, size_t width, size_t height, float start_time = 0,
             float end_time = 0) override;

  glm::vec3 sampleUv(Scene &scene, glm::vec2 uv, float time);

private:
  struct Tile {
    size_t i, j;
    std::vector<glm::vec3> pixels;
  };

  Tile snapTile(Scene &scene, size_t i, size_t j, size_t width, size_t height,
                float start_time, float end_time);

  void splatTile(Frame &frame, const Tile &tile);

  void sampleUvs(glm::vec2 min_uv, glm::vec2 max_uv,
                 std::vector<glm::vec2> &out);

  glm::vec3 trace(Ray ray, Scene &scene);
};

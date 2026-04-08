#pragma once

#include "Raytracer.h"
#include "Transform.h"
#include "glm/gtc/constants.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class PixelSampler {
public:
  virtual void sampleUvs(glm::vec2 uv_min, glm::vec2 uv_max,
                         std::vector<glm::vec2> &out) = 0;
};

class Frame {
public:
  float start_time;
  float end_time;
  size_t width, height;
  std::vector<uint8_t> pixels;

  void save(const std::string &path);
};

class Camera {
public:
  struct Projection {
    float near = 0.1;
    float far = 10.0;
    float fov = glm::half_pi<float>();
  };

  std::shared_ptr<Transform> transform;
  std::shared_ptr<PixelSampler> sampler;
  Projection projection;
  size_t width, height;

  Camera(std::shared_ptr<Transform> transform,
         std::shared_ptr<PixelSampler> sampler, Projection projection,
         size_t width, size_t height);

  Frame snap(std::shared_ptr<Raytracer> rt, float start_time = 0,
             float end_time = 0);

  glm::vec3 sampleUv(std::shared_ptr<Raytracer> rt, float time, float u,
                     float v);
  glm::vec3 pixel(std::shared_ptr<Raytracer> rt, float time, size_t x,
                  size_t y);

  void record(std::shared_ptr<Raytracer> rt, const std::string &out_dir,
              float start_time, float end_time, float fps);

private:
  struct Tile {
    size_t i;
    size_t j;
    std::vector<uint8_t> pixels;
  };

  static const size_t tile_size = 32;

  Tile shootTile(std::shared_ptr<Raytracer> rt, float start_time,
                 float end_time, size_t i, size_t j);

  void splatTile(Frame &frame, const Tile &tile);
};

class UniformPixelSampler : public PixelSampler {
public:
  size_t sample_cnt;

  UniformPixelSampler(size_t sample_cnt = 1);

  void sampleUvs(glm::vec2 uv_min, glm::vec2 uv_max,
                 std::vector<glm::vec2> &out) override;
};

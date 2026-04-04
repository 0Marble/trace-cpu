#include "Camera.h"
#include "../vendor/stb_image_write.h"
#include "Log.h"
#include "Progress.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/random.hpp"
#include <ctime>
#include <filesystem>
#include <vector>

#ifdef PARALLEL
#include <omp.h>
#endif

SimplePixelSampler::SimplePixelSampler(size_t sample_cnt)
    : sample_cnt(sample_cnt) {}

std::vector<glm::vec2> SimplePixelSampler::sampleUvs(glm::vec2 uv_min,
                                                     glm::vec2 uv_max) {
  return std::vector(sample_cnt, (uv_min + uv_max) * 0.5f);
}

void Frame::save(const std::string &path_str) {
  std::filesystem::path path(path_str);
  if (path.has_parent_path()) {
    std::filesystem::create_directories(path.parent_path());
  }

  int res =
      stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4);
  ASSERT(res);
}

Camera::Camera(std::shared_ptr<Transform> transform,
               std::shared_ptr<PixelSampler> sampler, Projection projection,
               size_t width, size_t height)
    : transform(transform), sampler(sampler), projection(projection),
      width(width), height(height) {}

Frame Camera::snap(std::shared_ptr<Raytracer> rt, float start_time,
                   float end_time) {
  rt->scene->build(start_time, end_time);

  Frame frame = {
      .start_time = start_time,
      .end_time = end_time,
      .width = width,
      .height = height,
      .pixels = std::vector<uint8_t>(width * height * 4, 0),
  };

  std::size_t x_tiles = width / tile_size;
  std::size_t y_tiles = height / tile_size;
  Progress::Task t =
      Progress::beginGroup((x_tiles + 1) * (y_tiles + 1), "tiles");

#ifdef PARALLEL
#pragma omp parallel for collapse(2)
#endif
  for (std::size_t i = 0; i <= x_tiles; i++) {
    for (std::size_t j = 0; j <= y_tiles; j++) {
      auto tile = shootTile(rt, start_time, end_time, i, j);
      splatTile(frame, tile);
    }
  }

  Progress::end();
  Progress::finish(t);

  return frame;
}

glm::vec3 Camera::sampleUv(std::shared_ptr<Raytracer> rt, float time, float u,
                           float v) {

  rt->scene->build(time, time);
  float aspect = (float)width / (float)height;
  glm::mat4 proj =
      glm::perspective(projection.fov, aspect, projection.near, projection.far);

  Ray ray = {.dir = glm::vec3(u, v, 1), .time = time};

  ray.origin = proj * glm::vec4(ray.origin, 1.0f);
  ray.dir = proj * glm::vec4(ray.dir, 0.0f);
  ray = transform->sample(time).apply(ray);
  ray.dir = glm::normalize(ray.dir);
  return rt->trace(ray);
}

glm::vec3 Camera::pixel(std::shared_ptr<Raytracer> rt, float t, size_t x,
                        size_t y) {
  float u = (float)(x) / (float)(width - 1) * 2.0f - 1.0f;
  float v = (float)(y) / (float)(height - 1) * 2.0f - 1.0f;
  return sampleUv(rt, t, u, v);
}

Camera::Tile Camera::shootTile(std::shared_ptr<Raytracer> rt, float start_time,
                               float end_time, size_t i, size_t j) {
  Progress::Task t = Progress::beginItem("tile (", i, ", ", j, ")");

  Tile tile = {
      .i = i,
      .j = j,
      .pixels = std::vector<uint8_t>(tile_size * tile_size * 4, 0),
  };

  float aspect = (float)width / (float)height;
  glm::mat4 proj =
      glm::perspective(projection.fov, aspect, projection.near, projection.far);

  for (size_t x = tile.i * tile_size; x < (tile.i + 1) * tile_size && x < width;
       x++) {
    for (size_t y = tile.j * tile_size;
         y < (tile.j + 1) * tile_size && y < height; y++) {

      glm::vec3 color(0);
      float u_min = (float)(x) / (float)(width) * 2.0f - 1.0f;
      float v_min = (float)(y) / (float)(height) * 2.0f - 1.0f;
      float u_max = (float)(x + 1) / (float)(width) * 2.0f - 1.0f;
      float v_max = (float)(y + 1) / (float)(height) * 2.0f - 1.0f;

      auto samples =
          sampler->sampleUvs(glm::vec2(u_min, v_min), glm::vec2(u_max, v_max));
      for (auto uv : samples) {
        float time = glm::linearRand(start_time, end_time);
        Ray ray = {.dir = glm::vec3(uv, 1), .time = time};

        ray.origin = proj * glm::vec4(ray.origin, 1.0f);
        ray.dir = proj * glm::vec4(ray.dir, 0.0f);
        ray = transform->sample(time).apply(ray);
        ray.dir = glm::normalize(ray.dir);

        color += rt->trace(ray);
      }

      glm::vec3 avg_color =
          glm::clamp(color / (float)samples.size(), glm::vec3(0), glm::vec3(1));
      size_t t_idx =
          (y - tile.j * tile_size) * (tile_size) + (x - tile.i * tile_size);

      tile.pixels[4 * t_idx + 0] = avg_color.r * 255.0f;
      tile.pixels[4 * t_idx + 1] = avg_color.g * 255.0f;
      tile.pixels[4 * t_idx + 2] = avg_color.b * 255.0f;
      tile.pixels[4 * t_idx + 3] = 0xFF;
    }
  }

  Progress::finish(t);

  return tile;
}

void Camera::splatTile(Frame &frame, const Tile &tile) {
  for (size_t x = tile.i * tile_size; x < (tile.i + 1) * tile_size && x < width;
       x++) {
    for (size_t y = tile.j * tile_size;
         y < (tile.j + 1) * tile_size && y < height; y++) {
      size_t t_idx =
          (y - tile.j * tile_size) * (tile_size) + (x - tile.i * tile_size);
      size_t f_idx = (height - y - 1) * width + x;

      frame.pixels[4 * f_idx + 0] = tile.pixels[4 * t_idx + 0];
      frame.pixels[4 * f_idx + 1] = tile.pixels[4 * t_idx + 1];
      frame.pixels[4 * f_idx + 2] = tile.pixels[4 * t_idx + 2];
      frame.pixels[4 * f_idx + 3] = tile.pixels[4 * t_idx + 3];
    }
  }
}

void Camera::record(std::shared_ptr<Raytracer> rt, const std::string &out_dir,
                    float start_time, float end_time, float fps) {
  float duration = end_time - start_time;

  float frame_dur = 1.0f / fps;
  size_t frame_cnt = std::ceil(duration * fps);

  Progress::Task t = Progress::beginGroup(frame_cnt, "video@", out_dir);
  for (size_t i = 0; i < frame_cnt; i++) {
    Progress::Task t = Progress::beginGroup(1, "frame ", i);

    float start = (float)i * frame_dur;
    float end = (float)(i + 1) * frame_dur;
    Frame f = snap(rt, start, end);
    f.save(out_dir + "/frame-" + std::to_string(i) + ".png");
    Progress::end();
    Progress::finish(t);
  }
  Progress::end();
  Progress::finish(t);
}

UniformPixelSampler::UniformPixelSampler(size_t sample_cnt)
    : sample_cnt(sample_cnt) {}

std::vector<glm::vec2> UniformPixelSampler::sampleUvs(glm::vec2 uv_min,
                                                      glm::vec2 uv_max) {
  std::vector<glm::vec2> res(sample_cnt, glm::vec2(0));

  for (auto &v : res) {
    glm::vec2 rand = glm::linearRand(uv_min, uv_max);
    v = rand;
  }

  return res;
}


#include "CpuTracer.h"
#include "Intersection.h"
#include "Log.h"
#include "Progress.h"
#include "Random.h"
#include "glm/ext/vector_float3.hpp"
#include <vector>

CpuTracer::CpuTracer(size_t bounce_cnt, size_t sample_cnt)
    : bounce_cnt(bounce_cnt), sample_cnt(sample_cnt) {}

Frame CpuTracer::snap(Scene &scene, size_t width, size_t height,
                      float start_time, float end_time) {
  Frame frame = {
      .width = width,
      .height = height,
      .start_time = start_time,
      .end_time = end_time,
      .pixels = std::vector<glm::vec3>(width * height),
  };

  std::size_t x_tiles = width / tile_size;
  std::size_t y_tiles = height / tile_size;
  Progress::Task t =
      Progress::beginGroup((x_tiles + 1) * (y_tiles + 1), "tiles");

#ifdef PARALLEL
#pragma omp parallel for collapse(2) schedule(dynamic, 1)
#endif
  for (std::size_t i = 0; i <= x_tiles; i++) {
    for (std::size_t j = 0; j <= y_tiles; j++) {
      auto tile = snapTile(scene, i, j, width, height, start_time, end_time);
      splatTile(frame, tile);
    }
  }

  Progress::endGroup();
  Progress::finish(t);

  return frame;
}

CpuTracer::Tile CpuTracer::snapTile(Scene &scene, size_t i, size_t j,
                                    size_t width, size_t height,
                                    float start_time, float end_time) {
  Progress::Task t = Progress::beginItem("tile (", i, ", ", j, ")");

  Tile tile = {
      .i = i,
      .j = j,
      .pixels = std::vector<glm::vec3>(tile_size * tile_size, glm::vec3(0.0f)),
  };

  std::vector<glm::vec2> samples{};
  for (size_t x = tile.i * tile_size; x < (tile.i + 1) * tile_size && x < width;
       x++) {
    for (size_t y = tile.j * tile_size;
         y < (tile.j + 1) * tile_size && y < height; y++) {

      glm::vec3 color(0);
      float u_min = (float)(x) / (float)(width) * 2.0f - 1.0f;
      float v_min = (float)(y) / (float)(height) * 2.0f - 1.0f;
      float u_max = (float)(x + 1) / (float)(width) * 2.0f - 1.0f;
      float v_max = (float)(y + 1) / (float)(height) * 2.0f - 1.0f;

      sampleUvs(glm::vec2(u_min, v_min), glm::vec2(u_max, v_max), samples);

      for (auto uv : samples) {
        float time = Random::uniform() * (end_time - start_time) + start_time;
        Ray ray = scene.camera.shootRay(uv, time);
        color += trace(ray, scene);
      }

      glm::vec3 avg_color =
          glm::clamp(color / (float)samples.size(), glm::vec3(0), glm::vec3(1));
      size_t t_idx =
          (y - tile.j * tile_size) * (tile_size) + (x - tile.i * tile_size);

      tile.pixels[t_idx] = avg_color;
    }
  }

  Progress::finish(t);

  return tile;
}

void CpuTracer::splatTile(Frame &frame, const Tile &tile) {
  for (size_t x = tile.i * tile_size;
       x < (tile.i + 1) * tile_size && x < frame.width; x++) {
    for (size_t y = tile.j * tile_size;
         y < (tile.j + 1) * tile_size && y < frame.height; y++) {
      size_t t_idx =
          (y - tile.j * tile_size) * (tile_size) + (x - tile.i * tile_size);
      size_t f_idx = (frame.height - y - 1) * frame.width + x;

      frame.pixels[f_idx] = tile.pixels[t_idx];
    }
  }
}

struct Step {
  Ray incoming;
  glm::vec3 direct_light = glm::vec3(0);
  glm::vec3 indirect_light = glm::vec3(0);
  size_t bounce = 0;
  glm::vec3 weight = glm::vec3(0);
  size_t secondary_ray_cnt = 0;
  size_t prev = 0;
  glm::vec3 color = glm::vec3(0);
};

glm::vec3 CpuTracer::trace(Ray from_camera_ray, Scene &scene) {
  scene.clearCache();

  static thread_local std::vector<Step> steps{bounce_cnt + 1};

  size_t read = 0;
  size_t write = 1;
  float continue_prob = 0.6;

  steps[0] = Step{.incoming = from_camera_ray};

  while (read != write) {
    Step &s = steps[read];
    read++;

    auto maybe_collisions = scene.intersect(s.incoming);
    if (!maybe_collisions) {
      continue;
    }

    Intersection col = maybe_collisions.value();

    InstantTransform transform = col.tri.transform()->sample(s.incoming.time);
    auto n2o = col.n2o;
    auto w2o = transform.asInv();
    auto o2w = transform.asMat();
    auto o2n = glm::transpose(n2o);

    Ray secondary_ray = s.incoming;
    glm::vec3 to_view_n = glm::normalize(
        o2n * (glm::vec3(w2o * glm::vec4(secondary_ray.dir, 0))));
    secondary_ray.origin = o2w * glm::vec4(col.ray.at(col.t), 1.0f);

    s.direct_light = glm::vec3(0);
    for (auto &light : scene.lights) {
      Ray shadow_ray = secondary_ray;
      glm::vec3 light_pos_w = light->worldPos(shadow_ray.time);
      shadow_ray.dir = glm::normalize(light_pos_w - shadow_ray.origin);

      if (scene.intersect(shadow_ray).has_value()) {
        continue;
      }

      auto light_color = light->sample(shadow_ray.dir, shadow_ray.time);
      if (!light_color)
        continue;

      glm::vec3 to_light_n =
          glm::normalize(o2n * (w2o * glm::vec4(shadow_ray.dir, 0)));

      s.direct_light += col.tri.material()->bsdf(col.uv, shadow_ray.time,
                                                 to_view_n, to_light_n) *
                        light_color.value() * std::max(to_light_n.z, 0.0f) /
                        glm::dot(light_pos_w - shadow_ray.origin,
                                 light_pos_w - shadow_ray.origin);
    }

    if (Random::uniform() > continue_prob || s.bounce >= bounce_cnt) {
      continue;
    }

    float pdf = 0.0f;
    glm::vec3 bsdf = glm::vec3(0.0f);
    glm::vec3 to_obj_n = col.tri.material()->sample(col.uv, secondary_ray.time,
                                                    to_view_n, pdf, bsdf);
    secondary_ray.dir = glm::normalize(o2w * glm::vec4(n2o * to_obj_n, 0.0f));

    Step next = {
        .incoming = secondary_ray,
        .bounce = s.bounce + 1,
        .weight = bsdf / pdf * std::max(to_obj_n.z, 0.0f) / continue_prob,
        .prev = read - 1,
    };

    ASSERT(write < steps.size());
    steps[write++] = next;
  }

  for (size_t j = 0; j < write; j++) {
    size_t i = write - j - 1;

    ASSERT(i < steps.size());
    Step &cur = steps[i];
    if (cur.secondary_ray_cnt == 0) {
      cur.color = cur.direct_light;
    } else {
      cur.color = cur.direct_light +
                  (cur.indirect_light) / (float)cur.secondary_ray_cnt;
    }
    if (cur.bounce == 0) {
      break;
    }

    ASSERT(cur.prev < steps.size());
    Step &prev = steps[cur.prev];
    prev.secondary_ray_cnt++;
    prev.indirect_light += cur.color * cur.weight;
  }

  return steps[0].color;
}

void CpuTracer::sampleUvs(glm::vec2 min_uv, glm::vec2 max_uv,
                          std::vector<glm::vec2> &out) {
  glm::vec2 size = max_uv - min_uv;
  for (size_t i = 0; i < sample_cnt; i++) {
    out.push_back(Random::uniform2() * size + min_uv);
  }
}

#include <cstddef>
#include <omp.h>

#include "BlinnPhongMaterial.h"
#include "Log.h"
#include "PointLight.h"
#include "Random.h"
#include "Raytracer.h"
#include "Scene.h"
#include "Sphere.h"
#include "Transform.h"
#include <memory>

#include "../vendor/stb_image_write.h"
#include "Triangle.h"

static const std::size_t tile_size = 32;
void renderTile(Raytracer &rt, std::size_t w, std::size_t h,
                std::size_t samples, std::size_t i, std::size_t j,
                std::vector<uint8_t> &out) {
  std::vector<glm::vec3> color(tile_size * tile_size, glm::vec3(0));

  for (std::size_t y = j * tile_size; y < h && y < (j + 1) * tile_size; y++) {
    for (std::size_t x = i * tile_size; x < w && x < (i + 1) * tile_size; x++) {

      std::size_t glob_idx = (w - y - 1) * w + x;
      std::size_t loc_idx =
          (y - j * tile_size) * tile_size + (x - i * tile_size);

      for (std::size_t k = 0; k < samples; k++) {
        float u = (float)(x) / (float)(w - 1) * 2.0f - 1.0f;
        float v = (float)(y) / (float)(h - 1) * 2.0f - 1.0f;

        glm::vec3 dir = {u, v, -1};
        dir = glm::normalize(dir);

        Ray ray = {.dir = dir};

        color[loc_idx] += rt.trace(ray);
      }

      glm::vec3 avg_color = glm::clamp(color[loc_idx] / (float)samples,
                                       glm::vec3(0), glm::vec3(1));
      out[4 * glob_idx + 0] = avg_color.r * 255.0;
      out[4 * glob_idx + 1] = avg_color.g * 255.0;
      out[4 * glob_idx + 2] = avg_color.b * 255.0;
      out[4 * glob_idx + 3] = 255;
    }
  }
}

void render(Raytracer &rt, std::size_t w, std::size_t h,
            std::size_t samples = 1) {
  std::vector<uint8_t> pixels(4 * w * h, 0);

  std::size_t x_tiles = w / tile_size;
  std::size_t y_tiles = h / tile_size;

  std::size_t total_work = (x_tiles + 1) * (y_tiles + 1);
  std::size_t progress = 0;

#ifdef PARALLEL
#pragma omp parallel for collapse(2)
#endif

  for (std::size_t i = 0; i <= x_tiles; i++) {
    for (std::size_t j = 0; j <= y_tiles; j++) {
      renderTile(rt, w, h, samples, i, j, pixels);

#ifdef PARALLEL
#pragma omp critical
#endif
      {
        progress += 1;
        LOG(LogLevel::LOG_INFO, progress, "/", total_work);
      }
    }
  }

  int res = stbi_write_png("image.png", w, h, 4, pixels.data(), w * 4);
  ASSERT(res);
}

static glm::vec3 quad[] = {
    glm::vec3(-1, -1, 0),
    glm::vec3(1, -1, 0),
    glm::vec3(1, 1, 0),
    glm::vec3(-1, 1, 0),
};

static int inds[] = {0, 1, 2, 0, 2, 3};

int main() {
  LOG(LogLevel::LOG_INFO, "running on", omp_get_num_procs(), "omp threads");

  auto raytracer = Raytracer();
  raytracer.rng = std::make_shared<Random>();
  raytracer.scene = std::make_shared<Scene>();

  auto green = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.1, 1.0, 0.1),
                                                    glm::vec3(0.0), 0);
  auto red = std::make_shared<BlinnPhongMaterial>(glm::vec3(1.0, 0.1, 0.1),
                                                  glm::vec3(0.0), 0);
  auto blue = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.1, 0.1, 1.0),
                                                   glm::vec3(0.0), 0);
  auto white =
      std::make_shared<BlinnPhongMaterial>(glm::vec3(1), glm::vec3(0), 0);

  auto back = std::make_shared<InstantTransform>(glm::vec3(0, 0, -4));
  auto bot = std::make_shared<InstantTransform>(
      glm::vec3(0, -1, -3), glm::vec3(1),
      glm::quat(0.7071068, -0.7071068, 0, 0));
  auto left =
      std::make_shared<InstantTransform>(glm::vec3(-1, 0, -3), glm::vec3(1),
                                         glm::quat(0.7071068, 0, 0.7071068, 0));
  auto sphere = std::make_shared<InstantTransform>(glm::vec3(0, -0.5, -3),
                                                   glm::vec3(0.5f));

  std::vector<std::shared_ptr<Triangle>> tris = {};
  for (size_t i = 0; i < sizeof(inds) / sizeof(inds[0]); i += 3) {
    auto tri = std::make_shared<Triangle>(quad[inds[i]], quad[inds[i + 1]],
                                          quad[inds[i + 2]]);
    tris.push_back(std::move(tri));
  }

  for (auto &tri : tris) {
    raytracer.scene->addObject(
        {.geometry = tri, .material = blue, .transform = bot});
    raytracer.scene->addObject(
        {.geometry = tri, .material = green, .transform = back});
    raytracer.scene->addObject(
        {.geometry = tri, .material = red, .transform = left});
  }

  raytracer.scene->addObject({
      .geometry = std::make_shared<Sphere>(),
      .material = white,
      .transform = sphere,
  });

  raytracer.scene->addLight(
      std::make_shared<PointLight>(glm::vec3(1, 1, 1), glm::vec3(0, 3, -2)));

  render(raytracer, 100, 100, 1);

  return 0;
}

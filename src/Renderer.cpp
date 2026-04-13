#include "Renderer.h"
#include "../vendor/stb_image_write.h"
#include "Log.h"
#include "Progress.h"
#include <cstdint>

void Frame::save(std::filesystem::path path) const {
  std::vector<uint8_t> data(width * height * 4, 0);

  for (size_t i = 0; i < pixels.size(); i++) {
    glm::vec3 color = pixels[i];
    data[4 * i + 0] = (color.r * 255.0f);
    data[4 * i + 1] = (color.g * 255.0f);
    data[4 * i + 2] = (color.b * 255.0f);
    data[4 * i + 3] = 0xFF;
  }

  int res =
      stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4);
  ASSERT(res);
}

std::vector<Frame> Renderer::record(Scene &scene, size_t width, size_t height,
                                    float start_time, float end_time,
                                    float fps) {
  std::vector<Frame> res = {};
  float duration = end_time - start_time;

  float frame_dur = 1.0f / fps;
  size_t frame_cnt = std::ceil(duration * fps);

  Progress::Task t = Progress::beginGroup(frame_cnt, "video");
  for (size_t i = 0; i < frame_cnt; i++) {
    Progress::Task t = Progress::beginGroup(1, "frame ", i);

    float start = (float)i * frame_dur;
    float end = (float)(i + 1) * frame_dur;
    Frame f = snap(scene, width, height, start, end);
    res.push_back(f);
    Progress::endGroup();
    Progress::finish(t);
  }
  Progress::endGroup();
  Progress::finish(t);

  return res;
}

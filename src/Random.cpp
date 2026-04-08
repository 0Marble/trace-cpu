#include "Random.h"

#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/constants.hpp"

#ifdef PARALLEL
#include <omp.h>
#endif

// splitmix64:
// https://prng.di.unimi.it/splitmix64.c
uint64_t splitmix_next(uint64_t &x) {
  uint64_t z = (x += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

static const float mul = 0x1.0p-24f;
using State = uint64_t[4];
#ifdef PARALLEL
static std::vector<State> per_thread_state;
#else
static State state;
#endif

static uint64_t next();

void Random::init(uint64_t seed) {
#ifdef PARALLEL
  per_thread_state = std::vector<State>(omp_get_max_threads());
  for (size_t i = 0; i < per_thread_state.size(); i++) {
    State &s = per_thread_state[i];
    uint64_t seed_local = seed + i;
    s[0] = splitmix_next(seed_local);
    s[1] = splitmix_next(seed_local);
    s[2] = splitmix_next(seed_local);
    s[3] = splitmix_next(seed_local);
  }
#else
  state[0] = splitmix_next(seed);
  state[1] = splitmix_next(seed);
  state[2] = splitmix_next(seed);
  state[3] = splitmix_next(seed);
#endif
}

float Random::uniform() {
  uint64_t x = next();
  return (uint32_t(x >> 32) >> 8) * mul;
}

glm::vec2 Random::uniform2() {
  uint64_t x = next();
  uint32_t a = x >> 32;
  uint32_t b = x & 0xFFFF'FFFF;
  return glm::vec2((a >> 8) * mul, (b >> 8) * mul);
}

glm::vec3 Random::hemisphere() {
  glm::vec2 phi_thera =
      uniform2() * glm::vec2(glm::two_pi<float>(), glm::pi<float>());

  float sin_phi = std::sin(phi_thera[0]);
  float cos_phi = std::cos(phi_thera[0]);

  float sin_theta = std::sin(phi_thera[1]);
  float cos_theta = std::cos(phi_thera[1]);

  return glm::vec3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
}

// xoshiro256+
// https://prng.di.unimi.it/xoshiro256plus.c
static inline uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

uint64_t next() {
#ifdef PARALLEL
  int thread = omp_get_thread_num();
  State &s = per_thread_state[thread];
#else
  State &s = state;
#endif

  const uint64_t result = s[0] + s[3];

  const uint64_t t = s[1] << 17;

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= t;

  s[3] = rotl(s[3], 45);

  return result;
}

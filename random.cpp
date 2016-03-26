#include "random.hpp"

namespace {
  std::random_device random_device;
  std::mt19937 global_rng(random_device());
}

size_t random_index(size_t elements)
{
  std::uniform_int_distribution<size_t> uni(0, elements - 1);
  return uni(global_rng);
}

// EOF

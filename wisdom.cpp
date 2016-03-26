#include "wisdom.hpp"
#include "random.hpp"

namespace {
  char const * const wisdom_db[] {
#include "wisdomdb.inc"
  };

  constexpr const size_t wisdom_entries = sizeof(wisdom_db)/sizeof(wisdom_db[0]);
}


std::string Wisdom::get() const
{
  if (wisdom_entries > 0) {
    return wisdom_db[random_index(wisdom_entries)];
  } else {
    return "Fresh out of wisdom!";
  }
}

// EOF

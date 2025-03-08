#include "chapters.hpp"

#include <iostream>
namespace ufsct {

  std::ostream &operator<< (std::ostream &os, validId const &id) {
    return os << id.ID;
  }
} // namespace ufsct
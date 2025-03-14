#include "chapters.hpp"

#include <iostream>
namespace ufsct {

  battleResult::battleResult (const chapter1 &c)
    : prepared{c.cityID},
      complete{battleComplete (c.tries)} {}
  battleResult::battleResult (const chapter2 &c)
    : prepared{c.cityID},
      complete{battleComplete (c.tries)} {}
  battleResult::battleResult (const chapter3 &c)
    : prepared{c.cityID},
      complete{battleComplete (c.tries)} {}

  auto battleComplete (std::span<const int, 2> tries) -> bool {
    return ( // first failed, any result in the second
      (tries[0] == chapter1::Fail && tries[1] != chapter1::NotFought) ||
      // Second not fought , success on the first
      (tries[1] == chapter1::NotFought &&
       (tries[0] != chapter1::Fail && tries[0] != chapter1::NotFought)));
  };

  auto operator<< (std::ostream &os, validId const &id) -> std::ostream & {
    return os << id.ID;
  }
} // namespace ufsct
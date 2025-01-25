#ifndef UFSCT_CAMPAIGN_H
#define UFSCT_CAMPAIGN_H
#include <array>
#include <string>
#include <string_view>

namespace ufsct {

  struct Chapter {
    std::array<std::string, 4> characters;
    std::array<std::string, 4> scenarios;
    std::array<std::string, 4> cities;
  };
  class Campaign {
    // static constexpr size_t noc = 4;
    static constexpr size_t noc = 1;
    std::array<Chapter, noc> chapters;

  public:
    Campaign (std::string_view);
    const Chapter &operator[] (size_t i) const;
  };
} // namespace ufsct
#endif // UFSCT_CAMPAIGN_H

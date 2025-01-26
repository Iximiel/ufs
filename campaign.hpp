#ifndef UFSCT_CAMPAIGN_H
#define UFSCT_CAMPAIGN_H
#include <array>
#include <string>
#include <string_view>

namespace ufsct {

  struct Chapter {
    std::array<std::string, 4> characters;
    std::array<std::string, 4> scenarios;
    std::array<std::string, 5> cities;
  };
  class Campaign {
    static constexpr size_t noc = 3;
    std::array<std::string, 12> characters;
    std::array<std::string, 12> scenarios;
    std::array<std::string, 14> cities;

  public:
    Campaign (std::string_view);
    // gets the data relative to the chapter
    Chapter operator[] (size_t i) const;
    // gets the absolute index from the characters
    std::string getCity (size_t i) const;
    std::string getScenario (size_t i) const;
    std::string getCharacter (size_t i) const;
  };
} // namespace ufsct
#endif // UFSCT_CAMPAIGN_H

#include <fstream>

#include "campaign.hpp"

#include "djson/json_read.hpp"

namespace ufsct {

  std::string campaingElement (
    const djson::Object &campaign,
    size_t               chapter,
    std::string          category,
    size_t               index) {
    return std::get<djson::String> (
      std::get<djson::Object> (campaign.get<djson::Array> ("chapters")[chapter])
        .get<djson::Array> (category)[index]);
  }

  Campaign::Campaign (std::string_view filename) {
    std::ifstream file (filename.data ());
    auto          campaign = djson::read (file);

    for (size_t i = 0; i < noc; i++) {
      size_t shift     = i * 4;
      size_t cityShift = (i == 0) ? 0 : (4 + 5 * (i - 1));
      for (size_t j = 0; j < 4; j++) {
        characters[shift + j] = campaingElement (*campaign, i, "characters", j);
        scenarios[shift + j]  = campaingElement (*campaign, i, "scenarios", j);
        cities[cityShift + j] = campaingElement (*campaign, i, "cities", j);
      }
      if (i != 0) {
        cities[cityShift + 4] = campaingElement (*campaign, i, "cities", 4);
      }
    }
  }

  Chapter Campaign::operator[] (size_t const i) const {
    Chapter toRet;
    size_t  shift     = i * 4;
    size_t  cityShift = (i == 0) ? 0 : (4 + 5 * (i - 1));
    for (size_t j = 0; j < 4; j++) {
      toRet.characters[j] = characters[shift + j];
      toRet.scenarios[j]  = scenarios[shift + j];
      toRet.cities[j]     = cities[cityShift + j];
    }
    if (i != 0) {
      toRet.cities[4] = cities[cityShift + 4];
    }

    return toRet;
  }
  std::string Campaign::getCity (size_t i) const { return cities.at (i); }
  std::string Campaign::getScenario (size_t i) const {
    return scenarios.at (i);
  }
  std::string Campaign::getCharacter (size_t i) const {
    return characters.at (i);
  }

} // namespace ufsct
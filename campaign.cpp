#include <fstream>

#include "campaign.hpp"

#include "djson/json_read.hpp"

namespace ufsct {

  std::string campaingElement (
    const djson::Object &campaign,
    int chapter,
    std::string category,
    int index) {
    return std::get<djson::String> (
      std::get<djson::Object> (campaign.get<djson::Array> ("chapters")[chapter])
        .get<djson::Array> (category)[index]);
  }

  Campaign::Campaign (std::string_view filename) {
    std::ifstream file (filename.data ());
    auto campaign = djson::read (file);

    for (size_t i = 0; i < noc; i++) {
      for (size_t j = 0; j < 4; j++) {
        chapters[i].characters[j] =
          campaingElement (*campaign, i, "characters", j);
        chapters[i].scenarios[j] =
          campaingElement (*campaign, i, "scenarios", j);
        chapters[i].cities[j] = campaingElement (*campaign, i, "cities", j);
      }
    }
  }

  const Chapter &Campaign::operator[] (size_t const i) const {
    return chapters[i];
  }
} // namespace ufsct
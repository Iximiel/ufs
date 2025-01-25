#include "save.hpp"

#include "djson/json_read.hpp"
#include "djson/json_write.hpp"

#include <fstream>

namespace ufsct {

  bool Save::load (std::string_view filename) {
    std::ifstream file (filename.data ());
    auto saveRead = djson::read (file);
    if (!saveRead) {
      return false;
    }
    auto save = *saveRead;

    auto chars = save.get<djson::Array> ("characters");
    for (int i = 0; i < characters.size (); i++) {
      characters[i] = chars.get<djson::String> (i);
    }

    auto citiesLoad = save.get<djson::Array> ("cities");
    for (int i = 0; i < citiesLoad.size (); i++) {
      cities[i] = citiesLoad.get<djson::String> (i);
    }
    lastBattle = save.get<djson::Number> ("lastBattle");

    auto destroyedCitiesLoad = save.get<djson::Array> ("destroyedCities");
    for (int i = 0; i < destroyedCitiesLoad.size (); i++) {
      destroyedCities[i] = destroyedCitiesLoad.get<djson::Boolean> (i);
    }
    auto chapter1 = save.get<djson::Object> ("chapter1");

    auto bt1 = chapter1.get<djson::Object> ("battle1");
    firstChapter[0].cityID = bt1.get<djson::Number> ("cityID");
    firstChapter[0].charID = bt1.get<djson::Number> ("charID");
    firstChapter[0].tries[0] =
      bt1.get<djson::Array> ("tries").get<djson::Number> (0);
    firstChapter[0].tries[1] =
      bt1.get<djson::Array> ("tries").get<djson::Number> (1);
    return true;
  }

  bool Save::save (std::string_view filename) {
    djson::Object data;
    // save["baseDifficulty"] = djson::Number (baseDifficulty);
    // save["currentPlay"] = djson::Number (currentPlay);
    data["characters"] = djson::Array{};
    for (const auto &c : characters) {
      data.get<djson::Array> ("characters").emplace_back (c);
    }
    data["cities"] = djson::Array{};
    for (const auto &c : cities) {
      data.get<djson::Array> ("cities").emplace_back (c);
    }

    djson::Object history;
    history["lastBattle"] = djson::Number (lastBattle);
    history["destroyedCities"] = djson::Array{};
    for (const auto &c : destroyedCities) {
      history.get<djson::Array> ("destroyedCities").emplace_back (c);
    }
    history["chapter1"] = djson::Object{};

    djson::Object bt1;
    bt1["cityID"] = djson::Number (firstChapter[0].cityID);
    bt1["charID"] = djson::Number (firstChapter[0].charID);
    bt1["tries"] = djson::Array (
      {djson::Number (firstChapter[0].tries[0]),
       djson::Number (firstChapter[0].tries[1])});
    history.get<djson::Object> ("chapter1")["battle1"] = bt1;

    // history.get<djson::Object>("chapter1")["battle2"] = djson::Object{};

    // history["chapter2"] = djson::Object{};
    // history["chapter3"] = djson::Object{};
    // history["chapter4"] = djson::Object{};

    djson::Object save;
    save["data"] = data;
    save["history"] = history;
    std::ofstream file (filename.data ());

    djson::write (file, save);
    return true;
  }

} // namespace ufsct
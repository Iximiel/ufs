#include "save.hpp"

#include "djson/json_read.hpp"
#include "djson/json_write.hpp"

#include <fstream>
#include <iostream>

namespace ufsct {
  Save::Save () {

  };
  Save::~Save () = default;
  Save &Save::operator= (const Save &) = default;
  Save &Save::operator= (Save &&) = default;
  Save::Save (std::string_view cname) : Save () { name = cname; }

  chapter1 &Save::getFirstChapter (int const battle) {
    return firstChapter.at (battle);
  }
  chapter2 &Save::getSecondChapter (int const battle) {
    return secondChapter.at (battle);
  }
  chapter3 &Save::getThirdChapter (int const battle) {
    return thirdChapter.at (battle);
  }

  const chapter1 &Save::getFirstChapter (int const battle) const {
    return firstChapter.at (battle);
  }
  const chapter2 &Save::getSecondChapter (int const battle) const {
    return secondChapter.at (battle);
  }
  const chapter3 &Save::getThirdChapter (int const battle) const {
    return thirdChapter.at (battle);
  }

  int Save::getRandomCharacterID (int const ch, int const index) const {
    // assert (index <4);
    std::cerr << "index ch" << 4 * ch + index << std::endl;
    return randomCharacterIDs.at (4 * ch + index);
  }
  int Save::getRandomCityID (int const ch, int const index) const {
    if (ch == 0) {
      // assert (index <4);
      std::cerr << "index ci" << index << std::endl;
      return randomCitiesIDs.at (index);
    }
    // assert (index <5);
    std::cerr << "index sc" << 4 + 5 * (ch - 1) + index << std::endl;
    return randomCitiesIDs.at (4 + 5 * (ch - 1) + index);
  }
  int Save::getRandomScenarioID (int const ch, int const index) const {
    // assert (index <4);
    std::cerr << "index " << 4 * ch + index << std::endl;
    return randomScenariosIDs.at (4 * ch + index);
  }

  template <typename T>
  T loadChapter (djson::Object chapter) {
    T toret;
    toret.cityID = chapter.get<djson::Number> ("cityID");
    toret.charID = chapter.get<djson::Number> ("charID");
    toret.sceneID = chapter.get<djson::Number> ("sceneID");
    auto tries = chapter.get<djson::Array> ("tries");
    toret.tries[0] = tries.get<djson::Number> (0);
    toret.tries[1] = tries.get<djson::Number> (1);
    if constexpr (std::is_same_v<T, chapter2>) {
      toret.elitecharID = chapter.get<djson::Number> ("elitecharID1");
    }
    if constexpr (std::is_same_v<T, chapter3>) {
      toret.elitecharID = chapter.get<djson::Number> ("elitecharID1");
      toret.elitecharID2 = chapter.get<djson::Number> ("elitecharID2");
    }
    return toret;
  }

  bool Save::load (std::string_view filename) {
    std::ifstream file (filename.data ());
    auto saveRead = djson::read (file);
    if (!saveRead) {
      return false;
    }
    name = saveRead->get<djson::String> ("name");

    auto history = saveRead->get<djson::Object> ("history");
    for (const auto &key : history.Keys ()) {
      if (key == "chapter1") {
        auto tmp = history.get<djson::Array> ("chapter1");
        firstChapter[0] = loadChapter<chapter1> (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          firstChapter[1] = loadChapter<chapter1> (tmp.get<djson::Object> (1));
        }
      } else if (key == "chapter2") {
        auto tmp = history.get<djson::Array> ("chapter2");
        secondChapter[0] = loadChapter<chapter2> (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          secondChapter[1] = loadChapter<chapter2> (tmp.get<djson::Object> (1));
        }
      } else if (key == "chapter3") {
        auto tmp = history.get<djson::Array> ("chapter3");
        thirdChapter[0] = loadChapter<chapter3> (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          thirdChapter[1] = loadChapter<chapter3> (tmp.get<djson::Object> (1));
        }
      } /*else if (key == "chapter4") {
        auto tmp = history.get<djson::Array> ("chapter4");
        fourthChapter[0] = loadChapter<chapter4>  (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          fourthChapter[1] = loadChapter<chapte4>  (tmp.get<djson::Object>
      (1));
        }
      }*/
    }
    return true;
  }

  djson::Object saveChapter (chapter1 c) {
    djson::Object toret;
    toret["cityID"] = djson::Number (c.cityID);
    toret["charID"] = djson::Number (c.charID);
    toret["sceneID"] = djson::Number (c.sceneID);
    toret["tries"] =
      djson::Array{{djson::Number (c.tries[0]), djson::Number (c.tries[1])}};
    return toret;
  }

  djson::Object saveChapter (chapter2 c) {
    auto toret = saveChapter (chapter1 (c));
    toret["elitecharID1"] = djson::Number (c.elitecharID);
    return toret;
  }

  djson::Object saveChapter (chapter3 c) {
    auto toret = saveChapter (chapter2 (c));
    toret["elitecharID2"] = djson::Number (c.elitecharID);
    return toret;
  }

  bool Save::save (std::string_view filename) {
    djson::Object history;

    if (firstChapter[0].charID != -1) {
      djson::Array ch1;
      ch1.push_back (saveChapter (firstChapter[0]));
      if (firstChapter[1].charID != -1) {
        ch1.push_back (saveChapter (firstChapter[1]));
      }
      history["chapter1"] = ch1;
    }

    if (secondChapter[0].charID != -1) {
      djson::Array ch2;
      ch2.push_back (saveChapter (secondChapter[0]));
      if (secondChapter[1].charID != -1) {
        ch2.push_back (saveChapter (secondChapter[1]));
      }
      history["chapter2"] = ch2;
    }

    if (thirdChapter[0].charID != -1) {
      djson::Array ch3;
      ch3.push_back (saveChapter (thirdChapter[0]));
      if (thirdChapter[1].charID != -1) {
        ch3.push_back (saveChapter (thirdChapter[1]));
      }
      history["chapter3"] = ch3;
    }
    /*
        if (fourthChapter[0].charID != -1) {
          djson::Array ch4;
          ch4.push_back (saveChapter (fourthChapter[0]));
          if (fourthChapter[1].charID != -1) {
            ch4.push_back (saveChapter (fourthChapter[1]));
          }
          history["chapter4"] = ch4;
        }
    */
    djson::Object save;
    save["name"] = djson::String{name};
    save["history"] = history;
    std::ofstream file (filename.data ());
    djson::write (file, save);

    return true;
  }
  std::array<int, 8> Save::getSurvivedCities () {
    std::array<int, 8> toret;
    toret[0] = firstChapter[0].cityID;
    toret[1] = firstChapter[1].cityID;
    toret[2] = secondChapter[0].cityID;
    toret[3] = secondChapter[1].cityID;
    toret[4] = thirdChapter[0].cityID;
    toret[5] = thirdChapter[1].cityID;
    // the fifth city of the second draft is saved for later
    toret[6] = randomCitiesIDs[8];
    // the fifth city of the third draft is saved is saved for later
    toret[6] = randomCitiesIDs[13];
    // sort in order go have the -1 in the end
    std::sort (toret.begin (), toret.end (), [] (auto a, auto b) {
      if (a == -1) {
        return false;
      }
      if (b == -1) {
        return true;
      }
      return a < b;
    });
    return toret;
  }

  void Save::setFistChapter (
    std::span<int, 4> citiesID,
    std::span<int, 4> charID,
    std::span<int, 4> sceneID) {
    std::copy (citiesID.begin (), citiesID.end (), randomCitiesIDs.begin ());
    std::copy (charID.begin (), charID.end (), randomCharacterIDs.begin ());
    std::copy (sceneID.begin (), sceneID.end (), randomScenariosIDs.begin ());
  }

  void Save::setSecondChapter (
    std::span<int, 5> citiesID,
    std::span<int, 4> charID,
    std::span<int, 4> sceneID) {
    std::copy (
      citiesID.begin (), citiesID.end (), randomCitiesIDs.begin () + 4);
    std::copy (charID.begin (), charID.end (), randomCharacterIDs.begin () + 4);
    std::copy (
      sceneID.begin (), sceneID.end (), randomScenariosIDs.begin () + 4);
  }

  void Save::setThirdChapter (
    std::span<int, 5> citiesID,
    std::span<int, 4> charID,
    std::span<int, 4> sceneID) {
    std::copy (
      citiesID.begin (), citiesID.end (), randomCitiesIDs.begin () + 9);
    std::copy (charID.begin (), charID.end (), randomCharacterIDs.begin () + 8);
    std::copy (
      sceneID.begin (), sceneID.end (), randomScenariosIDs.begin () + 8);
  }
} // namespace ufsct
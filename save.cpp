#include "save.hpp"

#include "djson/json_read.hpp"
#include "djson/json_write.hpp"

#include <algorithm>
#include <fstream>
#include <numeric>

#include <iostream>

namespace ufsct {
  Save::Save () {
    std::iota (randomCharacterIDs.begin (), randomCharacterIDs.end (), 0);
    std::iota (randomCitiesIDs.begin (), randomCitiesIDs.end (), 0);
    std::iota (randomScenariosIDs.begin (), randomScenariosIDs.end (), 0);
  };
  Save::~Save ()                       = default;
  Save &Save::operator= (const Save &) = default;
  Save &Save::operator= (Save &&)      = default;
  Save::Save (std::string_view cname) : Save () { name = cname; }

  std::string Save::getCampaignName () const { return name; }

  chapter3 Save::getChapter (int chapter, int battle) {
    switch (chapter) {
    case 0:
      return chapter3 (firstChapter.at (battle));
      break;
    case 1:
      return chapter3 (secondChapter.at (battle));
      break;
    case 2:
      return thirdChapter.at (battle);
      break;
    default:
      break;
    }
    return chapter3 ();
  }

  void Save::setTry (int chapter, int battle, int tryNumber, int difficulty) {
    switch (chapter) {
    case 0:
      firstChapter.at (battle).tries[tryNumber] = difficulty;
      break;
    case 1:
      secondChapter.at (battle).tries[tryNumber] = difficulty;
      break;
    case 2:
      thirdChapter.at (battle).tries[tryNumber] = difficulty;
      break;
    default:
      break;
    }
  }

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
    return randomCharacterIDs.at (4 * ch + index);
  }
  int Save::getRandomCityID (int const ch, int const index) const {
    if (ch == 0) {
      // assert (index <4);
      return randomCitiesIDs.at (index);
    }
    // assert (index <5);
    return randomCitiesIDs.at (4 + 5 * (ch - 1) + index);
  }
  int Save::getRandomScenarioID (int const ch, int const index) const {
    // assert (index <4);
    return randomScenariosIDs.at (4 * ch + index);
  }

  template <typename T>
  T loadChapter (djson::Object chapter) {
    T toret;
    toret.cityID   = chapter.get<djson::Number> ("cityID");
    toret.charID   = chapter.get<djson::Number> ("charID");
    toret.sceneID  = chapter.get<djson::Number> ("sceneID");
    auto tries     = chapter.get<djson::Array> ("tries");
    toret.tries[0] = tries.get<djson::Number> (0);
    toret.tries[1] = tries.get<djson::Number> (1);
    if constexpr (std::is_same_v<T, chapter2>) {
      toret.elitecharID = chapter.get<djson::Number> ("elitecharID1");
    }
    if constexpr (std::is_same_v<T, chapter3>) {
      toret.elitecharID  = chapter.get<djson::Number> ("elitecharID1");
      toret.elitecharID2 = chapter.get<djson::Number> ("elitecharID2");
    }
    return toret;
  }

  bool Save::load (std::string_view filename) {
    std::ifstream file (filename.data ());
    auto          saveRead = djson::read (file);
    if (!saveRead) {
      return false;
    }
    name = saveRead->get<djson::String> ("name");
    auto i_randomCharacterIDs =
      saveRead->get<djson::Array> ("randomCharacterIDs");
    auto i_randomCitiesIDs = saveRead->get<djson::Array> ("randomCitiesIDs");
    auto i_randomScenariosIDs =
      saveRead->get<djson::Array> ("randomScenariosIDs");
    for (size_t i = 0; i < 14; i++) {
      randomCitiesIDs[i] = i_randomCitiesIDs.get<djson::Number> (i);
      if (i < 12) {
        randomCharacterIDs[i] = i_randomCharacterIDs.get<djson::Number> (i);
        randomScenariosIDs[i] = i_randomScenariosIDs.get<djson::Number> (i);
      }
    }
    auto history = saveRead->get<djson::Object> ("history");
    for (const auto &key : history.Keys ()) {
      if (key == "chapter1") {
        auto tmp        = history.get<djson::Array> ("chapter1");
        firstChapter[0] = loadChapter<chapter1> (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          firstChapter[1] = loadChapter<chapter1> (tmp.get<djson::Object> (1));
        }
      } else if (key == "chapter2") {
        auto tmp         = history.get<djson::Array> ("chapter2");
        secondChapter[0] = loadChapter<chapter2> (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          secondChapter[1] = loadChapter<chapter2> (tmp.get<djson::Object> (1));
        }
      } else if (key == "chapter3") {
        auto tmp        = history.get<djson::Array> ("chapter3");
        thirdChapter[0] = loadChapter<chapter3> (tmp.get<djson::Object> (0));
        if (tmp.size () > 1) {
          thirdChapter[1] = loadChapter<chapter3> (tmp.get<djson::Object> (1));
        }
      } else if (key == "chapter4") {
        auto tmp = history.get<djson::Object> ("chapter4");
        if (auto k = tmp.Keys ();
            std::find (k.begin (), k.end (), "team") != k.end ()) {
          auto theTeam = tmp.get<djson::Array> ("team");
          for (size_t i = 0; i < theTeam.size (); i++) {
            victoryTeam[i] = theTeam.get<djson::Number> (i);
          }
          lastBattleScore = tmp.get<djson::Number> ("score");
        }
        auto tmp2 = tmp.get<djson::Array> ("cities");
        for (size_t i = 0; i < tmp2.size (); i++) {
          lastBattle[i] = tmp2.get<djson::Number> (i);
        }
      }
    }
    return true;
  }

  djson::Object saveChapter (chapter1 c) {
    djson::Object toret;
    toret["cityID"]  = djson::Number (c.cityID);
    toret["charID"]  = djson::Number (c.charID);
    toret["sceneID"] = djson::Number (c.sceneID);
    toret["tries"] =
      djson::Array{{djson::Number (c.tries[0]), djson::Number (c.tries[1])}};
    return toret;
  }

  djson::Object saveChapter (chapter2 c) {
    auto toret            = saveChapter (chapter1 (c));
    toret["elitecharID1"] = djson::Number (c.elitecharID);
    return toret;
  }

  djson::Object saveChapter (chapter3 c) {
    auto toret            = saveChapter (chapter2 (c));
    toret["elitecharID2"] = djson::Number (c.elitecharID2);
    return toret;
  }

  bool Save::save (std::string_view filename) {
    djson::Object history;
    std::cerr << "save to " << filename << std::endl;
    if (firstChapter[0].charID != -1) {
      std::cerr << "save firstChapter" << std::endl;
      djson::Array ch1;
      ch1.push_back (saveChapter (firstChapter[0]));
      if (firstChapter[1].charID != -1) {
        ch1.push_back (saveChapter (firstChapter[1]));
      }
      history["chapter1"] = std::move (ch1);
    }

    if (secondChapter[0].charID != -1) {
      djson::Array ch2;
      std::cerr << "save secondChapter" << std::endl;
      ch2.push_back (saveChapter (secondChapter[0]));
      if (secondChapter[1].charID != -1) {
        ch2.push_back (saveChapter (secondChapter[1]));
      }
      history["chapter2"] = std::move (ch2);
    }

    if (thirdChapter[0].charID != -1) {
      std::cerr << "save thirdChapter" << std::endl;
      djson::Array ch3;
      ch3.push_back (saveChapter (thirdChapter[0]));
      if (thirdChapter[1].charID != -1) {
        ch3.push_back (saveChapter (thirdChapter[1]));
      }
      history["chapter3"] = std::move (ch3);
    }
    if (lastBattle[0] != -1) {
      djson::Object ch4;
      if (victoryTeam[0] != -1) {
        auto team = djson::Array{};
        team.emplace_back (djson::Number (victoryTeam[0]));
        team.emplace_back (djson::Number (victoryTeam[1]));
        team.emplace_back (djson::Number (victoryTeam[2]));
        ch4["team"]  = team;
        ch4["score"] = djson::Number (lastBattleScore);
      }
      djson::Array cities;
      for (int i = 0; i < lastBattle.size (); ++i) {
        if (lastBattle[i] == -1) {
          break;
        }
        cities.push_back (djson::Number (lastBattle[i]));
      }
      ch4["cities"]       = cities;
      history["chapter4"] = ch4;
    }

    djson::Object save;
    save["name"] = djson::String{name};
    djson::Array export_randomCitiesIDs;
    export_randomCitiesIDs.resize (14);
    djson::Array export_randomCharacterIDs;
    export_randomCharacterIDs.resize (12);
    djson::Array export_randomScenariosIDs;
    export_randomScenariosIDs.resize (12);
    for (int i = 0; i < 14; ++i) {
      export_randomCitiesIDs[i] = djson::Number (randomCitiesIDs[i]);
      if (i < 12) {
        export_randomCharacterIDs[i] = djson::Number (randomCharacterIDs[i]);
        export_randomScenariosIDs[i] = djson::Number (randomScenariosIDs[i]);
      }
    }
    save["randomCitiesIDs"]    = std::move (export_randomCitiesIDs);
    save["randomCharacterIDs"] = std::move (export_randomCharacterIDs);
    save["randomScenariosIDs"] = std::move (export_randomScenariosIDs);

    save["history"] = std::move (history);

    std::ofstream file (filename.data ());
    djson::write (file, save);

    return true;
  }

  int citySurvived (chapter1 c) {
    // assuming that both the tries have happened
    if (c.tries[0] != chapter1::Fail || c.tries[1] != chapter1::Fail) {
      return c.cityID;
    }
    return -1;
  }

  std::array<int, 8> Save::getSurvivedCities () const {
    std::array<int, 8> toret;
    toret[0] = citySurvived (firstChapter[0]);
    toret[1] = citySurvived (firstChapter[1]);
    toret[2] = citySurvived (secondChapter[0]);
    toret[3] = citySurvived (secondChapter[1]);
    toret[4] = citySurvived (thirdChapter[0]);
    toret[5] = citySurvived (thirdChapter[1]);
    // the fifth city of the second draft is saved for later
    toret[6] = randomCitiesIDs[8];
    // the fifth city of the third draft is saved is saved for later
    toret[7] = randomCitiesIDs[13];
    // removing the destroyed cities in the last battle
    for (auto i = 0u; i < lastBattle.size (); i++) {
      if (lastBattle[i] == -1) {
        break;
      }
      if (auto it = std::find (toret.begin (), toret.end (), lastBattle[i]);
          it != toret.end ()) {
        *it = -1;
      }
    }
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
    for (int i = 0; i < 4; ++i) {
      std::cerr << randomCharacterIDs[i] << " ";
    }
    std::cerr << std::endl;
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

  /// @brief  the ide of the last battle with stored settings:
  ///
  /// -1 is empty campaing, 6 is final battle
  /// @return
  int Save::getLastBattlePrepared () const {
    if (
      lastBattle[0] != -1 ||
      (thirdChapter[1].cityID != -1 &&
       ( // won at first try
         (thirdChapter[1].tries[0] != chapter1::NotFought &&
          thirdChapter[1].tries[0] != chapter1::Fail) ||
         // any outcome for second try
         (thirdChapter[1].tries[0] != chapter1::NotFought &&
          thirdChapter[1].tries[1] == chapter1::Fail)))) {
      return 6;
    }
    // find the "id" of the last prepared scenario by looking if the city has
    // been selected
    int currentBattle = -1;
    for (int i = 0; i < 2; ++i) {
      if (firstChapter[i].cityID != -1) {
        ++currentBattle;
      }
      if (secondChapter[i].cityID != -1) {
        ++currentBattle;
      }
      if (thirdChapter[i].cityID != -1) {
        ++currentBattle;
      }
    }
    return currentBattle;
  }
  bool Save::calculateBattle (int scenario) const {
    // given a scenario, returns the last battle fought
    // a battle is fought if the first try has been fought
    // or if the second try has been fought after a failed first try
    auto battleComplete = [] (std::array<int, 2> tries) -> bool {
      return ( // first failed, any result in the second
        (tries[0] == chapter1::Fail && tries[1] != chapter1::NotFought) ||
        // Second not fought , success on the first
        (tries[1] == chapter1::NotFought &&
         (tries[0] != chapter1::Fail && tries[0] != chapter1::NotFought)));
    };
    switch (scenario) {
    case 0:
      [[fallthrough]];
    case 1:
      return battleComplete (firstChapter[scenario].tries);
      break;
    case 2:
      [[fallthrough]];
    case 3:
      return battleComplete (secondChapter[scenario - 2].tries);
      break;
    case 4:
      [[fallthrough]];
    case 5:
      return battleComplete (thirdChapter[scenario - 4].tries);
      break;
    default:
      return false;
    }
  }
  std::vector<int> Save::getPossibleElites (int chapter) const {
    std::vector<int> toret;
    if (chapter == 0) {
      return toret;
    }
    toret.push_back (firstChapter[0].charID);
    toret.push_back (firstChapter[1].charID);
    if (chapter == 1) {
      // if (secondChapter[0].elitecharID != -1) -1 are not present
      toret.erase (
        std::remove (
          toret.begin (), toret.end (), secondChapter[0].elitecharID),
        toret.end ());

      return toret;
    }
    toret.push_back (secondChapter[0].charID);
    toret.push_back (secondChapter[1].charID);
    if (chapter == 2) {
      // if (thirdChapter[0].elitecharID != -1) -1 are not present
      toret.erase (
        std::remove (toret.begin (), toret.end (), thirdChapter[0].elitecharID),
        toret.end ());
      toret.erase (
        std::remove (
          toret.begin (), toret.end (), thirdChapter[0].elitecharID2),
        toret.end ());
      return toret;
    }
    toret.push_back (thirdChapter[0].charID);
    toret.push_back (thirdChapter[1].charID);
    return toret;
  }

  void Save::endCampaign (int city, int score, std::array<int, 3> &team) {
    // thisf functin assingthe city id to the first -1 in the lastBattle array
    // in the score menu the last battle has the signigicance of being the
    // victory city here
    chapter4BattleLost (city);

    for (auto i = 0u; i < team.size (); i++) {
      // use std::copy
      victoryTeam[i] = team[i];
    }
    lastBattleScore = score;
  }

  void Save::chapter4BattleLost (int city) {
    std::cerr << "chapter4BattleLost" << std::endl;
    std::cerr << "city: " << city << std::endl;
    auto it = std::find (lastBattle.begin (), lastBattle.end (), -1);

    std::cerr << "city: " << it << std::endl;
    if (it != lastBattle.end ()) {
      *it = city;
    }

    // you should not be here
  }
  bool Save::lastBattleComplete () const {
    if (thirdChapter[1].cityID == -1) {
      // need the third chapter to be finished
      return false;
    }
    if (victoryTeam[0] != -1) {
      return true;
    }
    auto sc = getSurvivedCities ();
    // if no ID are present then the campaign is over
    return std::all_of (
      sc.begin (), sc.end (), [] (auto a) { return a != -1; });
  }

  chapter4Report Save::getChapter4Results () const {
    std::cerr << "getChapter4Results" << std::endl;
    chapter4Report toret;
    // we put the battles that have been fought first
    std::vector<int> citiesDone;
    citiesDone.reserve (8);
    toret.score       = lastBattleScore;
    int currentBattle = 0;
    for (auto i = 0u; i < lastBattle.size (); i++) {
      if (lastBattle[i] == -1) {
        break;
      }
      toret.battles[currentBattle].cityID = lastBattle[i];
      citiesDone.push_back (lastBattle[i]);
      if (currentBattle > 0) {
        // if anohter battle has been fought, measn the previous city is lost
        toret.battles[currentBattle - 1].battleOutcome = -1;
        --toret.score;
      }
      toret.battles[currentBattle].battleOutcome = lastBattleScore;
      std::cerr << "currentBattle: " << currentBattle
                << " city: " << toret.battles[currentBattle].cityID
                << std::endl;
      ++currentBattle;
    }

    auto survivedCities = getSurvivedCities ();
    for (auto i = 0u; i < survivedCities.size (); i++) {
      if (survivedCities[i] == -1) {
        break;
      }
      if (
        std::find (citiesDone.begin (), citiesDone.end (), survivedCities[i]) ==
        citiesDone.end ()) {
        std::cerr << "currentBattle: " << currentBattle
                  << " city: " << toret.battles[currentBattle].cityID
                  << std::endl;
        toret.battles[currentBattle].cityID = survivedCities[i];
        // battle outcomoe is by default not fought
        ++currentBattle;
      }
    }
    toret.victoryTeam = victoryTeam;

    std::cerr << "readytoret\n";

    // and then we reduce the score fore ach lost battle, and we add the score
    // for each won battle

    // oop on the battles
    for (int i = 0; i < 2; ++i) {
      // the first try is either a -1 or the difficulty
      toret.score += firstChapter[i].tries[0];
      toret.score += secondChapter[i].tries[0];
      toret.score += thirdChapter[i].tries[0];
      // the second try should be skipped if is  not fought (in the oher case is
      // -1 or the score)
      if (firstChapter[i].tries[1] != chapter1::NotFought) {
        toret.score += firstChapter[i].tries[1];
      }
      if (secondChapter[i].tries[1] != chapter1::NotFought) {
        toret.score += secondChapter[i].tries[1];
      }
      if (thirdChapter[i].tries[1] != chapter1::NotFought) {
        toret.score += thirdChapter[i].tries[1];
      }
    }
    return toret;
  }

} // namespace ufsct
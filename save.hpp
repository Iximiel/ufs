#ifndef UFSCT_SAVE_H
#define UFSCT_SAVE_H
#include <array>
#include <span>
#include <string>
#include <string_view>
#include <vector>
namespace ufsct {

  struct chapter1 {
    constexpr static int NotFought = -2;
    constexpr static int Fail = -1;
    // The tries can have 3 states: NotFought, Fail, value difficulty in case of
    // victory
    std::array<int, 2> tries = {NotFought, NotFought};
    int cityID = -1;
    int charID = -1;
    int sceneID = -1;
  };

  struct chapter2 : public chapter1 {
    int elitecharID = -1;
  };

  struct chapter3 : public chapter2 {
    int elitecharID2 = -1;
  };

  class Save {
    std::string name;
    std::array<chapter1, 2> firstChapter;
    std::array<chapter2, 2> secondChapter;
    std::array<chapter3, 2> thirdChapter;
    // std::array<chapter4, 2> fourthChapter;
    std::array<int, 12> randomCharacterIDs;
    std::array<int, 12> randomScenariosIDs;
    std::array<int, 14> randomCitiesIDs;

  public:
    Save ();
    Save (std::string_view);
    Save &operator= (const Save &);
    Save &operator= (Save &&);
    ~Save ();
    bool load (std::string_view);
    bool save (std::string_view);
    std::array<int, 8> getSurvivedCities ();
    const chapter1 &getFirstChapter (int battle) const;
    const chapter2 &getSecondChapter (int battle) const;
    const chapter3 &getThirdChapter (int battle) const;
    chapter1 &getFirstChapter (int battle);
    chapter2 &getSecondChapter (int battle);
    chapter3 &getThirdChapter (int battle);

    int getLastBattlePrepared () const;
    bool calculateBattle (int scenario) const;

    int getRandomCharacterID (int ch, int index) const;
    int getRandomCityID (int ch, int index) const;
    int getRandomScenarioID (int ch, int index) const;
    void
      setFistChapter (std::span<int, 4>, std::span<int, 4>, std::span<int, 4>);
    void setSecondChapter (
      std::span<int, 5>, std::span<int, 4>, std::span<int, 4>);
    void
      setThirdChapter (std::span<int, 5>, std::span<int, 4>, std::span<int, 4>);
  };
} // namespace ufsct
#endif // UFSCT_SAVE_H
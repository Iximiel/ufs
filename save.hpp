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
    constexpr static int Fail      = -1;
    // The tries can have 3 states: NotFought, Fail, value difficulty in case of
    // victory
    std::array<int, 2> tries        = {NotFought, NotFought};
    int                cityID       = -1;
    int                charID       = -1;
    int                sceneID      = -1;
    int                elitecharID  = -1;
    int                elitecharID2 = -1;
  };

  // this is an experiment to use same data with different types, I do not know
  // if I am lazy or tired
  struct chapter2 : public chapter1 {};

  struct chapter3 : public chapter1 {};

  struct chapter4Report {
    int cityID        = -1;
    int battleOutcome = chapter1::NotFought;
  };

  class Save {
    std::string             name;
    std::array<chapter1, 2> firstChapter;
    std::array<chapter2, 2> secondChapter;
    std::array<chapter3, 2> thirdChapter;
    // std::array<chapter4, 2> fourthChapter;
    std::array<int, 12> randomCharacterIDs;
    std::array<int, 12> randomScenariosIDs;
    std::array<int, 14> randomCitiesIDs;
    std::array<int, 8>  lastBattle{-1, -1, -1, -1, -1, -1, -1, -1};
    std::array<int, 3>  victoryTeam{-1, -1, -1};
    int                 lastBattleScore{-1};

  public:
    Save ();
    Save (std::string_view);
    Save &operator= (const Save &);
    Save &operator= (Save &&);
    ~Save ();
    bool               load (std::string_view);
    bool               save (std::string_view);
    std::array<int, 8> getSurvivedCities () const;
    std::string        getCampaignName () const;
    // this gets  generally chapter
    chapter3 getChapter (int chapter, int battle);
    void     setTry (int chapter, int battle, int tryNumber, int difficulty);

    const chapter1 &getFirstChapter (int battle) const;
    const chapter2 &getSecondChapter (int battle) const;
    const chapter3 &getThirdChapter (int battle) const;
    chapter1       &getFirstChapter (int battle);
    chapter2       &getSecondChapter (int battle);
    chapter3       &getThirdChapter (int battle);

    int  getLastBattlePrepared () const;
    bool calculateBattle (int scenario) const;

    int getRandomCharacterID (int ch, int index) const;
    int getRandomCityID (int ch, int index) const;
    int getRandomScenarioID (int ch, int index) const;
    // a lazy way to write less code in the UI:
    template <int chapter, int cityDim>
    void setChapter (
      std::span<int, cityDim> citiesID,
      std::span<int, 4>       charID,
      std::span<int, 4>       sceneID) {
      if constexpr (chapter == 0) {
        setFistChapter (citiesID, charID, sceneID);
      } else if constexpr (chapter == 1) {
        setSecondChapter (citiesID, charID, sceneID);
      } else if constexpr (chapter == 2) {
        setThirdChapter (citiesID, charID, sceneID);
      }
    }

    void setFistChapter (
      std::span<int, 4> citiesID,
      std::span<int, 4> charID,
      std::span<int, 4> sceneID);

    void setSecondChapter (
      std::span<int, 5> citiesID,
      std::span<int, 4> charID,
      std::span<int, 4> sceneID);
    void setThirdChapter (
      std::span<int, 5> citiesID,
      std::span<int, 4> charID,
      std::span<int, 4> sceneID);
    std::vector<int> getPossibleElites (int chapter) const;
    void endCampaign (int city, int score, std::array<int, 3> &team);
    void chapter4BattleLost (int city);
    bool lastBattleComplete () const;
  };

} // namespace ufsct
#endif // UFSCT_SAVE_H
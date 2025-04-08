#ifndef UFSCT_SAVE_H
#define UFSCT_SAVE_H
#include <array>
#include <cassert>
#include <concepts>
#include <iosfwd>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "chapters.hpp"

namespace ufsct {

  class Save {
    std::string             name;
    std::array<chapter1, 2> firstChapter;
    std::array<chapter2, 2> secondChapter;
    std::array<chapter3, 2> thirdChapter;

    std::array<validId, 8> lastBattle{-1, -1, -1, -1, -1, -1, -1, -1};
    std::array<int, 3>     victoryTeam{-1, -1, -1};
    int                    lastBattleScore{-1};

    std::array<int, 12> randomCharacterIDs;
    std::array<int, 12> randomScenariosIDs;
    std::array<int, 14> randomCitiesIDs;

  public:
    Save ();
    Save (std::string_view);
    Save &operator= (const Save &);
    Save &operator= (Save &&);
    ~Save ();
    auto load (std::string_view) -> bool;
    auto save (std::string_view) -> bool;
    auto getCampaignName () const -> std::string;

    auto getSurvivedCities () const -> std::array<validId, 8>;

    auto getChapter4Results () const -> chapter4Report;

    // this gets  generally chapter
    auto getChapter (unsigned chapter, unsigned battle) -> chapter3;
    void setTry (
      unsigned chapter, unsigned battle, unsigned tryNumber, int difficulty);

    auto getFirstChapter (unsigned battle) const -> const chapter1 &;
    auto getSecondChapter (unsigned battle) const -> const chapter2 &;
    auto getThirdChapter (unsigned battle) const -> const chapter3 &;
    auto getFirstChapter (unsigned battle) -> chapter1 &;
    auto getSecondChapter (unsigned battle) -> chapter2 &;
    auto getThirdChapter (unsigned battle) -> chapter3 &;

    auto getBattleResult (unsigned chapter, unsigned battle) const
      -> std::optional<battleResult>;

    auto getRandomCharacterID (unsigned ch, unsigned index) const -> int;
    auto getRandomCityID (unsigned ch, unsigned index) const -> int;
    auto getRandomScenarioID (unsigned ch, unsigned index) const -> int;

    // a lazy way to write less code in the UI:
    template <unsigned chapter, unsigned cityDim>
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
    void endCampaign (unsigned city, int score, std::array<int, 3> &team);
    void chapter4BattleLost (unsigned city);
    auto lastBattleComplete () const -> bool;
  };

} // namespace ufsct
#endif // UFSCT_SAVE_H
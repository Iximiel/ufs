#ifndef UFSCT_SAVE_H
#define UFSCT_SAVE_H
#include <array>
#include <cassert>
#include <concepts>
#include <iosfwd>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace ufsct {

  // this class is not a "good idea", I think: is just a way for me to not write
  // static_cast over and over again.
  // I am doing this for fun an for learning something, so losing some thine
  // might be useful
  class validId {
  private:
    int ID = -1;

  public:
    validId () = default;
    validId (std::integral auto id) : ID{static_cast<int> (id)} {}
    operator int () const { return ID; }
    operator size_t () const {
      // this is bold
      assert (ID >= 0);
      return static_cast<size_t> (ID);
    }
    operator unsigned () const {
      // this is bold
      assert (ID >= 0);
      return static_cast<unsigned> (ID);
    }
    operator double () const { return static_cast<double> (ID); }
    auto operator<=> (validId const &other) const { return ID <=> other.ID; };
    bool operator== (std::integral auto const &other) const {
      return ID == static_cast<int> (other);
    };
    bool operator!= (std::integral auto const &other) const {
      return ID != static_cast<int> (other);
    };
    friend std::ostream &operator<< (std::ostream &os, validId const &id);
  };

  std::ostream &operator<< (std::ostream &os, validId const &id);

  struct chapter1 {
    constexpr static int NotFought = -2;
    constexpr static int Fail      = -1;
    // The tries can have 3 states: NotFought, Fail, value difficulty in case of
    // victory
    std::array<validId, 2> tries        = {NotFought, NotFought};
    validId                cityID       = -1;
    validId                charID       = -1;
    validId                sceneID      = -1;
    validId                elitecharID  = -1;
    validId                elitecharID2 = -1;
  };

  // this is an experiment to use same data with different types, I do not know
  // if I am lazy or tired
  struct chapter2 : public chapter1 {};

  struct chapter3 : public chapter1 {};

  struct chapter4battle {
    int cityID        = -1;
    int battleOutcome = chapter1::NotFought;
  };

  struct chapter4Report {
    std::array<chapter4battle, 8> battles;
    int                           score = 0;

    std::array<int, 3> victoryTeam{-1, -1, -1};
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
    bool        load (std::string_view);
    bool        save (std::string_view);
    std::string getCampaignName () const;

    std::array<validId, 8> getSurvivedCities () const;

    chapter4Report getChapter4Results () const;

    // this gets  generally chapter
    chapter3 getChapter (unsigned chapter, unsigned battle);
    void
    setTry (unsigned chapter, unsigned battle, int tryNumber, int difficulty);

    const chapter1 &getFirstChapter (unsigned battle) const;
    const chapter2 &getSecondChapter (unsigned battle) const;
    const chapter3 &getThirdChapter (unsigned battle) const;
    chapter1       &getFirstChapter (unsigned battle);
    chapter2       &getSecondChapter (unsigned battle);
    chapter3       &getThirdChapter (unsigned battle);

    int  getLastBattlePrepared () const;
    bool calculateBattle (int scenario) const;

    int getRandomCharacterID (unsigned ch, unsigned index) const;
    int getRandomCityID (unsigned ch, unsigned index) const;
    int getRandomScenarioID (unsigned ch, unsigned index) const;
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
    bool lastBattleComplete () const;
  };

} // namespace ufsct
#endif // UFSCT_SAVE_H
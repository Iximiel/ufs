#ifndef UFSCT_CHAPTERS_H
#define UFSCT_CHAPTERS_H
#include <array>
#include <cassert>
#include <iosfwd>

namespace ufsct {

  // this class is not a "good idea", I think: is just a way for me to not write
  // static_cast over and over again.
  // I am doing this for fun an for learning something, so losing some time
  // might be useful
  class validId {
  private:
    int ID = -1;

  public:
    validId ()                           = default;
    validId (validId &&)                 = default;
    validId (const validId &)            = default;
    validId &operator= (validId &&)      = default;
    validId &operator= (const validId &) = default;
    validId (std::convertible_to<int> auto id) : ID{static_cast<int> (id)} {}
    validId &operator= (std::convertible_to<int> auto id) {
      ID = static_cast<int> (id);
      return *this;
    }
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
    // auto operator<=> (const validId &other) const { return ID <=> other.ID;
    // };
    bool operator== (const validId &other) const { return ID == other.ID; };
    bool operator< (const validId &other) const { return ID < other.ID; };
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
    std::array<int, 2> tries        = {NotFought, NotFought};
    validId            cityID       = -1;
    validId            charID       = -1;
    validId            sceneID      = -1;
    validId            elitecharID  = -1;
    validId            elitecharID2 = -1;
  };

  // this is an experiment to use same data with different types, I do not know
  // if I am lazy or tired
  struct chapter2 : public chapter1 {};

  struct chapter3 : public chapter1 {};

  /// @brief a return type for the status of the battle, used in determining if
  /// build the new scenario or display the battle screen
  struct battleResult {
    bool prepared;
    bool complete;
    explicit battleResult (const chapter1 &c);
    explicit battleResult (const chapter2 &c);
    explicit battleResult (const chapter3 &c);
    battleResult (const battleResult &other)            = default;
    battleResult &operator= (const battleResult &other) = default;
  };

  struct chapter4battle {
    int cityID        = -1;
    int battleOutcome = chapter1::NotFought;
  };

  struct chapter4Report {
    std::array<chapter4battle, 8> battles;
    int                           score = 0;

    std::array<int, 3> victoryTeam{-1, -1, -1};
  };

} // namespace ufsct
#endif // UFSCT_CHAPTERS_H

#ifndef UFSCT_SAVE_H
#define UFSCT_SAVE_H
#include <array>
#include <string>
#include <string_view>
#include <vector>
namespace ufsct {

  struct chapter1 {
    int tries[2] = {-1, -1};
    int cityID;
    int charID;
  };
  /*
  struct chapter2 : public chapter1
  {
    int elitecharID;
  };

  struct chapter3: public chapter2{
  int elitecharID2;
  };
  */
  class Save {
    std::array<std::string, 6> cities = {};
    std::array<std::string, 6> characters = {};
    std::array<bool, 6> destroyedCities{false};
    std::array<chapter1, 2> firstChapter;
    int lastBattle;

  public:
    Save (/* args */);
    ~Save ();
    bool load (std::string_view);
    bool save (std::string_view);
  };
} // namespace ufsct
#endif // UFSCT_SAVE_H
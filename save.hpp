#ifndef UFSCT_SAVE_H
#define UFSCT_SAVE_H
#include <array>
#include <string>
#include <string_view>
#include <vector>
namespace ufsct {

  struct chapter1 {
    std::array<int, 2> tries = {-1, -1};
    int cityID;
    int charID = -1;
  };

  struct chapter2 : public chapter1 {
    int elitecharID;
  };

  struct chapter3 : public chapter2 {
    int elitecharID2;
  };

  class Save {
    std::string name;
    std::array<chapter1, 2> firstChapter;
    std::array<chapter2, 2> secondChapter;
    std::array<chapter3, 2> thirdChapter;
    // std::array<chapter4, 2> fourthChapter;

  public:
    Save ();
    Save (std::string_view);
    Save &operator= (const Save &);
    Save &operator= (Save &&);
    ~Save ();
    bool load (std::string_view);
    bool save (std::string_view);
  };
} // namespace ufsct
#endif // UFSCT_SAVE_H
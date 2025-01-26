#include "save.hpp"

#include "djson/json_read.hpp"
#include "djson/json_write.hpp"

#include <fstream>

namespace ufsct {
  Save::Save () = default;
  Save::~Save () = default;
  Save &Save::operator= (const Save &) = default;
  Save &Save::operator= (Save &&) = default;

  template <typename T>
  T loadChapter (djson::Object chapter) {
    T toret;
    toret.cityID = chapter.get<djson::Number> ("cityID");
    toret.charID = chapter.get<djson::Number> ("charID");
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

  Save::Save (std::string_view cname) : name (cname) {}

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
    toret["cityID"] = djson::Number{c.cityID};
    toret["charID"] = djson::Number{c.charID};
    toret["tries"] =
      djson::Array{{djson::Number{c.tries[0]}, djson::Number{c.tries[1]}}};
    return toret;
  }
  djson::Object saveChapter (chapter2 c) {
    auto toret = saveChapter (chapter1 (c));
    toret["elitecharID1"] = djson::Number{c.elitecharID};
    return toret;
  }

  djson::Object saveChapter (chapter3 c) {
    auto toret = saveChapter (chapter2 (c));
    toret["elitecharID2"] = djson::Number{c.elitecharID};
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

} // namespace ufsct
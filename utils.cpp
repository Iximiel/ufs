#include "utils.hpp"
namespace utils {
  // Define a special style for some menu entry.
  ftxui::MenuEntryOption Colored (ftxui::Color c) {
    using namespace ftxui;
    MenuEntryOption option;
    option.transform = [c] (EntryState state) {
      state.label = (state.active ? "> " : "  ") + state.label;
      Element e = text (state.label) | color (c);
      if (state.focused)
        e = e | inverted;
      if (state.active)
        e = e | bold;
      return e;
    };
    return option;
  }
} // namespace utils
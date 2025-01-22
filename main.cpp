
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class UnderFallingSkiesTracker {
  ftxui::Element mainGrid (ftxui::Element body) {
    using namespace ftxui;
    return vbox (
      {hbox (
         {filler (), text ("Under Falling Skies campaign tracker"), filler ()}),
       separator (), filler (), body, filler (), separator (),
       text ("Version 0.1")});
  }

public:
  int mainMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;

    const std::vector<std::string> menu_entries = {
      "New Game", "Load Game", "Exit"};
    int selection = 0;
    ftxui::MenuOption option;
    option.on_enter = screen.ExitLoopClosure ();
    auto menu = Menu (&menu_entries, &selection, option);
    auto layout = Container::Vertical (
      {Renderer ([] { return filler (); }), menu,
       Renderer ([] { return filler (); })});
    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox ({filler (), layout->Render (), filler ()}));
    }));
    return selection;
  }
  int loadMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    bool goBack = false;
    bool selection = true;
    int selection2 = 0;
    ftxui::MenuOption option;
    auto close = screen.ExitLoopClosure ();
    std::vector<std::string> FileAndDirs;
    std::filesystem::path path = std::filesystem::current_path ();
    std::string visual = path.string ();
    option.on_enter = [&] {
      auto selected = FileAndDirs[selection2];
      if (selected == "..") {
        path = path.parent_path ();
      } else {
        if (std::filesystem::is_directory (path / selected)) {
          path /= selected;
        } else {
          visual = (path / selected).string ();
        }
      }
      close ();
    };

    auto openButton = Button ("Open", [&] {
      // change
      auto selected = FileAndDirs[selection2];
      if (selected == "..") {
        path = path.parent_path ();
      } else {
        if (std::filesystem::is_directory (path / selected))
          path /= selected;
        else
          visual = (path / selected).string ();
      }
      visual = path.string ();
      // goBack = true;
      // selection = false;
      close ();
    });
    auto backButton = Button ("Back", [&] {
      goBack = true;
      selection = false;
      close ();
    });

    auto dirMenu = Menu (&FileAndDirs, &selection2, option);
    auto layout = Container::Horizontal (
      {dirMenu,
       Container::Vertical (
         {Renderer ([] { return filler (); }), openButton, backButton})});
    while (selection) {
      FileAndDirs.resize (1);
      FileAndDirs[0] = "..";
      for (auto &p : std::filesystem::directory_iterator (path)) {
        if (p.path ().extension () == ".json") {
          FileAndDirs.push_back (p.path ().filename ().string ());
        } else if (p.is_directory ()) {
          FileAndDirs.push_back (p.path ().stem ().string ());
        }
      }
      screen.Loop (Renderer (layout, [&] {
        return mainGrid (vbox ({
          text (visual) | border,
          hbox (
            {frame ({dirMenu->Render ()}), filler (),
             vbox ({filler (), openButton->Render (), backButton->Render ()})})
          // layout->Render ()
        }));
      }));
    }

    if (goBack) {
      return 0;
    }
    return 1;
  }
};

int main (int, char **) {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen ();
  UnderFallingSkiesTracker program;
  int selection = 0;
  while (true) {
    switch (selection) {
    case 0:
      selection = program.mainMenu (screen);
      break;
    case 1:
      selection = program.loadMenu (screen);
      break;
    default:
      break;
    }
    if (selection == 2) {
      break;
    }
  }

  return EXIT_SUCCESS;
}

using namespace ftxui;

ButtonOption Style () {
  auto option = ButtonOption::Animated ();
  option.transform = [] (const EntryState &s) {
    auto element = text (s.label);
    if (s.focused) {
      element |= bold;
    }
    return element | center | borderEmpty | flex;
  };
  return option;
}

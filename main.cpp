
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "campaign.hpp"

class UnderFallingSkiesTracker {
  ftxui::Element mainGrid (ftxui::Element body) {
    using namespace ftxui;
    return vbox (
      {hbox (
         {filler (), text ("Under Falling Skies campaign tracker"), filler ()}),
       separator (), body | flex, separator (), text ("Version 0.1")});
  }
  const ufsct::Campaign campaign{"campaign.data"};
  std::mt19937 rng{std::random_device{}()};

public:
  enum class navigation { ufsMain, ufsNew, ufsLoad, ufsExit };
  navigation mainMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    using enum navigation;
    const std::vector<std::string> menu_entries = {
      "New Game", "Load Game", "Exit"};
    navigation selection = ufsMain;
    auto close = screen.ExitLoopClosure ();
    auto newButton = Button ("New Game", [&] {
      selection = ufsNew;
      close ();
    });
    auto loadButton = Button ("Load Game", [&] {
      selection = ufsLoad;
      close ();
    });
    auto exitButton = Button ("Exit", [&] {
      selection = ufsExit;
      close ();
    });
    auto menu = Container::Vertical ({newButton, loadButton, exitButton});
    auto layout = Container::Vertical (
      {Renderer ([] { return filler (); }), menu,
       Renderer ([] { return filler (); })});
    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox ({filler (), layout->Render (), filler ()}));
    }));
    return selection;
  }

  navigation newCampaignMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;

    bool goBack = false;
    auto close = screen.ExitLoopClosure ();
    auto backButton = Button ("Back", [&] {
      goBack = true;
      close ();
    });

    int chosenID = -1;
    std::array<size_t, 4> charID = {0, 1, 2, 3};
    std::shuffle (charID.begin (), charID.end (), rng);
    std::array<size_t, 4> sceneID = {0, 1, 2, 3};
    std::shuffle (sceneID.begin (), sceneID.end (), rng);
    std::array<size_t, 4> citiesID = {0, 1, 2, 3};
    std::shuffle (citiesID.begin (), citiesID.end (), rng);
    Components setbttn (2);
    Elements battleChoiche;
    for (int i = 0; i < 2; i++) {
      setbttn[i] = Button ("Set " + std::to_string (i + 1), [&] {
        // goBack = true;
        chosenID = i;
        close ();
      });
      battleChoiche.push_back (vbox (
        vbox (
          text (campaign[0].characters[charID[i]]),
          text (campaign[0].scenarios[sceneID[i]]),
          text (campaign[0].cities[citiesID[i]])) |
          border,
        setbttn[i]->Render ()));
    }
    Element document = flexbox (
      battleChoiche, ftxui::FlexboxConfig ()
                       .Set (FlexboxConfig::Direction::Row)
                       .Set (FlexboxConfig::JustifyContent::Center));
    auto layout = Container::Horizontal (
      {Renderer ([&] { return document; }),
       Container::Vertical (
         {Renderer ([&] { return filler (); }), backButton})});

    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox ({filler (), layout->Render (), filler ()}));
    }));

    if (goBack) {
      return navigation::ufsMain;
    }
    return navigation::ufsMain;
  }

  navigation loadMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    bool goBack = false;
    bool selection = true;
    bool readyToLoad = false;
    int selection2 = 0;
    ftxui::MenuOption option;
    auto close = screen.ExitLoopClosure ();
    std::vector<std::string> FileAndDirs;
    std::filesystem::path path = std::filesystem::current_path ();
    std::string visual = path.string ();

    auto openButton = Maybe (
      Button (
        "Open",
        [&] {
          // change
          auto selected = FileAndDirs[selection2];
          if (selected == "..") {
            path = path.parent_path ();
          } else {
            if (!std::filesystem::is_directory (path / selected)) {
              visual = (path / selected).string ();
            }
          }
          visual = path.string ();
          // goBack = true;
          // selection = false;
          close ();
        }),
      &readyToLoad);

    auto backButton = Button ("Back", [&] {
      goBack = true;
      selection = false;
      close ();
    });

    option.on_enter = [&] {
      auto selected = FileAndDirs[selection2];
      readyToLoad = false;
      if (selected == "..") {
        path = path.parent_path ();
        visual = path.string ();
      } else {
        if (std::filesystem::is_directory (path / selected)) {
          path /= selected;
          visual = path.string ();
        } else {
          readyToLoad = true;
        }
      }
      close ();
    };

    option.on_change = [&] {
      auto selected = FileAndDirs[selection2];
      readyToLoad = !std::filesystem::is_directory (path / selected);
    };

    auto dirMenu = Menu (&FileAndDirs, &selection2, option);
    auto layout = Container::Horizontal (
      {dirMenu | flex,
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
          window (text ("path"), text (visual)),
          hbox (
            {frame ({dirMenu->Render ()}) | flex,
             vbox (
               {filler (), openButton->Render (), backButton->Render ()})}) |
            flex
          // layout->Render ()
        }));
      }));
    }

    if (goBack) {
      return navigation::ufsMain;
    }
    return navigation::ufsLoad;
  }
};

int main (int, char **) {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen ();
  UnderFallingSkiesTracker program;
  UnderFallingSkiesTracker::navigation selection =
    UnderFallingSkiesTracker::navigation::ufsMain;

  while (true) {
    switch (selection) {
      using enum UnderFallingSkiesTracker::navigation;
    case ufsMain:
      selection = program.mainMenu (screen);
      break;
    case ufsNew:
      selection = program.newCampaignMenu (screen);
      break;
    case ufsLoad:
      selection = program.loadMenu (screen);
      break;
    case ufsExit:
      [[fallthrough]];
    default:
      break;
    }
    if (selection == UnderFallingSkiesTracker::navigation::ufsExit) {
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

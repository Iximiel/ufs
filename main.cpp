
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "campaign.hpp"
#include "save.hpp"

class UnderFallingSkiesTracker {
  ftxui::Element mainGrid (ftxui::Element body) {
    using namespace ftxui;
    return vbox (
      {hbox (
         {filler (), text ("Under Falling Skies campaign tracker"), filler ()}),
       separator (), body | flex, separator (), text ("Version 0.1")});
  }
  const ufsct::Campaign campaign{"campaign.data"};
  ufsct::Save playerdata;
  std::mt19937 rng{std::random_device{}()};
  std::string file;
  std::string campaignName{"Campaign name"};
  void reset () {
    playerdata = ufsct::Save ();
    file = "";
    campaignName = "Campaign name";
  }

public:
  enum class navigation {
    ufsMain,
    ufsNew,
    ufsLoad,
    ufsExit,
    ufsSave,
    ufsBattle
  };

  navigation mainMenu (ftxui::ScreenInteractive &screen) {
    reset ();
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
    auto startButton = Button ("Start", [&] { close (); });

    auto campaignNameI = Input (&campaignName);

    auto layout = Container::Horizontal (
      {campaignNameI, Container::Vertical ({startButton, backButton})});

    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox (
        {filler (),
         hbox (
           {hbox ({text ("New name: "), campaignNameI->Render ()}) | border |
              flex,
            vbox ({filler (), startButton->Render (), backButton->Render ()})}),

         filler ()}));
    }));

    if (goBack) {
      campaignName = "Campaign Name";
      return navigation::ufsMain;
    }

    return navigation::ufsSave;
  }
  navigation saveMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    using namespace ftxui;
    bool goBack = false;

    int selection2 = 0;
    ftxui::MenuOption option;
    auto close = screen.ExitLoopClosure ();
    std::vector<std::string> FileAndDirs;
    std::filesystem::path path = std::filesystem::current_path ();
    std::string visual = path.string ();

    bool doSave = false;
    auto openButton = Button ("Save", [&] {
      // change
      auto selected = FileAndDirs[selection2];

      if (visual != path.string ()) {
        if (!std::filesystem::is_directory (visual)) {
          if (std::filesystem::path (visual).extension () != ".json") {
            visual += ".json";
          }
          doSave = true;
        }
        // do nothing
      }

      close ();
    });

    auto backButton = Button ("Back", [&] {
      goBack = true;
      close ();
    });

    option.on_enter = [&] {
      auto selected = FileAndDirs[selection2];
      if (selected == "..") {
        path = path.parent_path ();
      } else {
        if (std::filesystem::is_directory (path / selected)) {
          path /= selected;
        }
      }
      visual = path.string ();
      close ();
    };

    option.on_change = [&] { auto selected = FileAndDirs[selection2]; };

    auto dirMenu = Menu (&FileAndDirs, &selection2, option);
    auto filenameInput = Input (&visual);
    auto layout = Container::Horizontal (
      {filenameInput, dirMenu | flex,
       Container::Vertical (
         {Renderer ([] { return filler (); }), openButton, backButton})});
    while (!goBack && !doSave) {
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
        return mainGrid (vbox (
          {window (text ("Filename:"), filenameInput->Render ()),
           hbox (
             {frame ({dirMenu->Render ()}) | flex,
              vbox (
                {filler (), openButton->Render (), backButton->Render ()})}) |
             flex}));
      }));
    }

    if (goBack) {
      return navigation::ufsNew;
    }
    if (doSave) {
      file = visual;
      playerdata = ufsct::Save (campaignName);
      playerdata.save (file);
      return navigation::ufsBattle;
    }
    return navigation::ufsSave;
  }
  navigation battleMenu (ftxui::ScreenInteractive &screen) {
    // determine which battle have been prepared
    int lastPreparedBattle = playerdata.getLastBattlePrepared ();
    // determine if the last battle has been fought
    bool lastBattleFought = playerdata.calculateBattle (lastPreparedBattle);
    if (lastPreparedBattle < 2) {
      return battleChapter1Menu (screen);
    }

    return navigation::ufsMain;
  }

  navigation battleChapter1Menu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    std::cerr << "battleChapter1Menu" << std::endl;
    bool goBack = false;
    auto close = screen.ExitLoopClosure ();
    auto backButton = Button ("Exit", [&] {
      goBack = true;
      close ();
    });
    int start = 0;
    if (playerdata.getFirstChapter (0).charID != -1) {
      start = 2;
      std::cerr << "second battle" << std::endl;
    }
    int chosenID = -1;

    if (start == 0) {
      std::array<int, 4> charID = {0, 1, 2, 3};
      std::shuffle (charID.begin (), charID.end (), rng);
      std::array<int, 4> sceneID = {0, 1, 2, 3};
      std::shuffle (sceneID.begin (), sceneID.end (), rng);
      std::array<int, 4> citiesID = {0, 1, 2, 3};
      std::shuffle (citiesID.begin (), citiesID.end (), rng);
      playerdata.setFistChapter (citiesID, charID, sceneID);
    }
    Components setbttn (2);
    Elements battleChoice;
    for (int i = 0; i < 2; i++) {
      std::cerr << i << std::endl;
      setbttn[i] =
        Button ("Set " + std::to_string (i + 1), [&chosenID, &close, i, start] {
          chosenID = i + start;
          close ();
        });
      battleChoice.push_back (window (
        text ("Set " + std::to_string (i + 1)),
        vbox (
          window (
            text ("character"),
            text (campaign.getCharacter (
              playerdata.getRandomCharacterID (0, i + start)))),
          window (
            text ("scenario"),
            text (campaign.getScenario (
              playerdata.getRandomScenarioID (0, i + start)))),
          window (
            text ("city"),           //
            text (campaign.getCity ( //
              playerdata.getRandomCityID (0, i + start)))))));
    }

    auto layout = Container::Horizontal (
      {Renderer ([&] { return battleChoice[0]; }),
       Renderer ([&] { return battleChoice[1]; }),

       Container::Vertical (
         {Renderer ([&] { return filler (); }), setbttn[0], setbttn[1],
          backButton})});

    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox ({filler (), layout->Render (), filler ()}));
    }));

    if (goBack) {
      return navigation::ufsMain;
    }
    // so that becames 1 or 0
    start /= 2;
    std::cerr << "chosenID: " << chosenID << std::endl;
    std::cerr << "charID    : " << playerdata.getRandomCharacterID (0, chosenID)
              << "\n";
    std::cerr << "scenarioID: " << playerdata.getRandomScenarioID (0, chosenID)
              << "\n";
    std::cerr << "cityID    : " << playerdata.getRandomCityID (0, chosenID)
              << "\n";

    playerdata.getFirstChapter (start).charID =
      playerdata.getRandomCharacterID (0, chosenID);
    playerdata.getFirstChapter (start).sceneID =
      playerdata.getRandomScenarioID (0, chosenID);
    playerdata.getFirstChapter (start).cityID =
      playerdata.getRandomCityID (0, chosenID);

    playerdata.save (file);
    return navigation::ufsBattle;
  }

  navigation loadMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    bool goBack = false;
    bool readyToLoad = false;
    bool fileSet = false;
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
              file = (path / selected).string ();

              fileSet = true;
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
          visual = (path / selected).string ();
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
    while (!goBack && !fileSet) {
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
    if (fileSet) {
      playerdata.load (file);
      return navigation::ufsBattle;
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
    case ufsBattle:
      selection = program.battleMenu (screen);
      break;
    case ufsSave:
      selection = program.saveMenu (screen);
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

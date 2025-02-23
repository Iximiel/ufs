
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
  ufsct::Save           playerdata;
  std::mt19937          rng{std::random_device{}()};
  std::string           file;
  std::string           campaignName{"Campaign name"};
  void                  reset () {
    playerdata   = ufsct::Save ();
    file         = "";
    campaignName = "Campaign name";
  }

  ftxui::Element chapterSummary (int chapter) {
    using namespace ftxui;
    std::array<Element, 2> bt;
    for (int battle = 0; battle < 2; battle++) {
      auto        tdata        = playerdata.getChapter (chapter, battle);
      std::string charName     = campaign.getCharacter (tdata.charID);
      std::string scenarioName = campaign.getScenario (tdata.sceneID);
      std::string cityName     = campaign.getCity (tdata.cityID);
      Elements    data         = {};
      std::string try1         = "X";
      std::string try2         = "X";
      bool        destroyed    = tdata.tries[0] == ufsct::chapter1::Fail &&
                       tdata.tries[1] == ufsct::chapter1::Fail;
      if (!destroyed) {
        if (tdata.tries[0] == ufsct::chapter1::Fail) {
          try2 = std::to_string (tdata.tries[1]);
        } else {
          try1 = std::to_string (tdata.tries[0]);
          try2 = " ";
        }
      }
      auto pointsquare = [] (const std::string &s, int sizeVal) {
        // the plus 2 is arbitrayr to make it look a square, may depend on the
        // font
        return text (s) | center | size (WIDTH, EQUAL, 2 + sizeVal) |
               size (HEIGHT, EQUAL, sizeVal) | border;
      };

      auto category = [] (const std::string &s) {
        return text (s) | vcenter | size (WIDTH, EQUAL, 11);
      };

      data.push_back (hbox (
        category ("City:"), separator (), paragraph (cityName) | vcenter,
        pointsquare (try1, 1), pointsquare (try2, 1)));
      data.push_back (hbox (
        category ("Scenario:"), separator (),
        paragraph (scenarioName) | vcenter));
      data.push_back (hbox (
        category ("Character:"), separator (), text (charName) | vcenter));

      if (chapter > 0) {
        data.push_back (hbox (
          category ("Elite:"), separator (),
          text (campaign.getCharacter (tdata.elitecharID)) | vcenter));
      }
      if (chapter > 1) {
        data.push_back (hbox (
          category ("Elite:"), separator (),
          text (campaign.getCharacter (tdata.elitecharID)) | vcenter));
      }

      bt[battle] = vbox (data);
    }
    return hbox (
      bt[0] | size (WIDTH, EQUAL, 40) | border,
      bt[1] | size (WIDTH, EQUAL, 40) | border);
  }

  ftxui::Element finalChapterSummary () {
    using namespace ftxui;
    // to future me [](){} == []{}
    auto pointsquare = [] (const std::string &s, int sizeVal) {
      // the plus 2 is arbitrayr to make it look a square, may depend on the
      // font
      return text (s) | center | size (WIDTH, EQUAL, 2 + sizeVal) |
             size (HEIGHT, EQUAL, sizeVal) | border;
    };

    Elements Cities;
    auto     ch4 = playerdata.getChapter4Results ();

    for (auto city : ch4.battles) {
      if (city.cityID > -1) {
        std::string cityName = campaign.getCity (city.cityID);
        std::string points   = [] (int p) -> std::string {
          if (p == ufsct::chapter1::NotFought) {
            return " ";
          }
          if (p == ufsct::chapter1::Fail) {
            return "X";
          }
          return std::to_string (p);
        }(city.battleOutcome);
        Cities.push_back (hbox (
          text ("City:") | vcenter | size (WIDTH, EQUAL, 8), separator (),
          text (cityName) | size (WIDTH, EQUAL, 18) | vcenter,
          pointsquare (points, 1)));
      }
    }

    if (ch4.victoryTeam[0] != -1) {
      Cities.push_back (window (
        text ("Victory Team"),
        vbox (
          text (campaign.getCharacter (ch4.victoryTeam[0])),
          text (campaign.getCharacter (ch4.victoryTeam[1])),
          text (campaign.getCharacter (ch4.victoryTeam[2])))));
    }
    Cities.push_back (window (
      text ("Final Score"), text (std::to_string (ch4.score)) | center));
    return vbox (Cities);
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
    navigation selection  = ufsMain;
    auto       close      = screen.ExitLoopClosure ();
    auto       newButton  = Button ("New Game", [&] {
      selection = ufsNew;
      close ();
    });
    auto       loadButton = Button ("Load Game", [&] {
      selection = ufsLoad;
      close ();
    });
    auto       exitButton = Button ("Exit", [&] {
      selection = ufsExit;
      close ();
    });
    auto       menu = Container::Vertical ({newButton, loadButton, exitButton});
    auto       layout = Container::Vertical (
      {Renderer ([] { return filler (); }), menu,
             Renderer ([] { return filler (); })});
    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox ({filler (), layout->Render (), filler ()}));
    }));
    return selection;
  }
  navigation newCampaignMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;

    bool goBack      = false;
    auto close       = screen.ExitLoopClosure ();
    auto backButton  = Button ("Back", [&] {
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

    int                      selection2 = 0;
    ftxui::MenuOption        option;
    auto                     close = screen.ExitLoopClosure ();
    std::vector<std::string> FileAndDirs;
    std::filesystem::path    path   = std::filesystem::current_path ();
    std::string              visual = path.string ();

    bool doSave     = false;
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

    auto dirMenu       = Menu (&FileAndDirs, &selection2, option);
    auto filenameInput = Input (&visual);
    auto layout        = Container::Horizontal (
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
      file       = visual;
      playerdata = ufsct::Save (campaignName);
      playerdata.save (file);
      return navigation::ufsBattle;
    }
    return navigation::ufsSave;
  }
  navigation battleMenu (ftxui::ScreenInteractive &screen) {
    std::cerr << "battleMenu" << std::endl;
    if (playerdata.lastBattleComplete ()) {
      return summaryMenu (screen);
    }
    // determine which battle have been prepared
    int lastPreparedBattle = playerdata.getLastBattlePrepared ();
    std::cerr << "lastPreparedBattle: " << lastPreparedBattle << std::endl;
    // determine if the last battle has been fought
    bool lastPreparedBattleCompleted =
      playerdata.calculateBattle (lastPreparedBattle);
    std::cerr << "lastPreparedBattleFought: "
              << (lastPreparedBattleCompleted ? "true" : "false") << std::endl;
    if (
      !lastPreparedBattleCompleted && lastPreparedBattle >= 0 &&
      lastPreparedBattle < 5) {
      // this means that we have the data
      std::cerr << "we have the data" << std::endl;
      return battleScoreMenu (screen);
    }
    if (lastPreparedBattleCompleted) {
      // increment to go to the next round
      lastPreparedBattle++;
    }
    if (lastPreparedBattle < 2) { // 0 1
      return battleChapterMenu<0> (screen);
    }
    if (lastPreparedBattle < 4) { // 2 6
      if (!lastPreparedBattleCompleted) {
        return battleScoreMenu (screen);
      }
      return battleChapterMenu<1> (screen);
    }
    if (lastPreparedBattle < 6) { // 4 5
      if (!lastPreparedBattleCompleted) {
        return battleScoreMenu (screen);
      }
      return battleChapterMenu<2> (screen);
    }
    return finalBattle (screen);
  }

  navigation summaryMenu (ftxui::ScreenInteractive &screen) {
    std::cerr << "summaryMenu" << std::endl;
    using namespace ftxui;
    auto close      = screen.ExitLoopClosure ();
    auto backButton = Button ("Exit", [&] { close (); });

    std::array<Element, 3>   ch;
    int                      chselected = 0;
    std::vector<std::string> chstr      = {
      "Chapter 1",
      "Chapter 2",
      "Chapter 3",
      "Chapter 4",
    };

    auto tab_menu      = Menu (&chstr, &chselected);
    auto tab_container = Container::Tab (
      {Renderer ([&] { return chapterSummary (0); }),
       Renderer ([&] { return chapterSummary (1); }),
       Renderer ([&] { return chapterSummary (2); }),
       Renderer ([&] { return finalChapterSummary (); })},
      &chselected);

    auto activeElements =
      Container::Horizontal ({tab_menu, tab_container, backButton});

    std::cerr << "Loop\n";
    screen.Loop (Renderer (activeElements, [&] {
      return mainGrid (hbox (
        {filler (),
         hbox (
           hbox (tab_menu->Render (), separator (), tab_container->Render ()),
           //  vbox (ch[0], ch[1], ch[2]) | yframe,
           vbox (filler (), backButton->Render ())),
         filler ()}));
    }));
    return navigation::ufsMain;
  }

  navigation finalBattle (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    std::cerr << "finalBattle" << std::endl;
    bool goBack             = false;
    bool next               = false;
    bool win                = false;
    bool charactersSelected = false;
    bool citySeleceted      = false;
    bool ready              = false;
    auto close              = screen.ExitLoopClosure ();
    auto backButton         = Button ("Exit", [&] {
      goBack = true;
      close ();
    });
    auto winButton          = Maybe (
      Button (
        "Win",
        [&] {
          win = true;
          close ();
        }),
      &ready);
    auto failButton = Maybe (
      Button (
        "Fail",
        [&] {
          win = false;
          close ();
        }),
      &ready);

    // character selection
    CheckboxOption      elitesch;
    auto                elitesChoiceIDs = playerdata.getPossibleElites (3);
    std::array<bool, 6> elitesChoice;
    elitesch.on_change = [&] {
      charactersSelected =
        std::count (elitesChoice.begin (), elitesChoice.end (), true) == 3;
      ready = charactersSelected && citySeleceted;
    };
    auto characters = Container::Vertical ({});
    for (int i = 0; i < elitesChoiceIDs.size (); i++) {
      elitesChoice[i] = false;
      characters->Add (Checkbox (
        campaign.getCharacter (elitesChoiceIDs[i]), &elitesChoice[i],
        elitesch));
    }

    // city selection
    auto           cities = Container::Vertical ({});
    CheckboxOption citysch;
    auto           citiesChoiceIDs = playerdata.getSurvivedCities ();
    std::array<bool, citiesChoiceIDs.size ()> citiesChoice;
    citysch.on_change = [&] {
      citySeleceted =
        std::count (citiesChoice.begin (), citiesChoice.end (), true) == 1;
      ready = charactersSelected && citySeleceted;
    };
    for (int i = 0; i < citiesChoiceIDs.size (); i++) {
      citiesChoice[i] = false;
      if (citiesChoiceIDs[i] != -1) {
        cities->Add (Checkbox (
          campaign.getCity (citiesChoiceIDs[i]), &citiesChoice[i], citysch));
      }
    }
    ///
    std::string battlescore = "0";
    Component   battreRes   = Input (&battlescore);

    auto activeElements = Container::Horizontal (
      {battreRes, winButton, failButton, backButton, characters, cities});
    screen.Loop (Renderer (activeElements, [&] {
      return mainGrid (hbox (
        {filler (),
         hbox (
           window (text ("Cities"), cities->Render () | flex),
           window (text ("Characters (3)"), characters->Render () | flex),
           hbox (filler (), window (text ("difficulty"), battreRes->Render ())),
           vbox (
             filler (), winButton->Render (), failButton->Render (),
             backButton->Render ())),
         filler ()}));
    }));
    if (goBack) {
      return navigation::ufsMain;
    }

    std::cerr << "\t>";
    for (int i = 0; i < citiesChoice.size (); i++) {
      std::cerr << citiesChoice[i] << std::flush;
    }
    std::cerr << "\n";
    std::cerr << "\t>";
    for (int i = 0; i < citiesChoiceIDs.size (); i++) {
      std::cerr << citiesChoiceIDs[i] << " " << std::flush;
    }
    std::cerr << "\n";
    auto city = citiesChoiceIDs
      [std::find (citiesChoice.begin (), citiesChoice.end (), true) -
       citiesChoice.begin ()];
    std::cerr << "city: " << city << std::endl;
    if (win) {
      std::cerr << "Win: " << battlescore << std::endl;
      std::cerr << "In" << campaign.getCity (city) << std::endl;
      std::array<int, 3> team;
      for (int i = 0, k = 0; i < elitesChoice.size (); i++) {
        if (elitesChoice[i]) {
          team[k] = elitesChoiceIDs[i];
          std::cerr << "\t" << campaign.getCharacter (team[k]) << std::endl;
          ++k;
        }
      }
      auto score = std::stoi (battlescore);
      playerdata.endCampaign (city, score, team);
      playerdata.save (file);
      return navigation::ufsBattle;
    }
    std::cerr << "Lost" << campaign.getCity (citiesChoiceIDs[city])
              << std::endl;
    playerdata.chapter4BattleLost (city);
    playerdata.save (file);
    return navigation::ufsBattle;
  }

  navigation battleScoreMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    std::cerr << "battleScoreMenu" << std::endl;
    bool goBack     = false;
    bool next       = false;
    bool lost       = false;
    auto close      = screen.ExitLoopClosure ();
    auto backButton = Button ("Exit", [&] {
      goBack = true;
      close ();
    });
    auto winButton  = Button ("Win", [&] {
      next = true;
      close ();
    });
    auto loseButton = Button ("Lost", [&] {
      lost = true;
      next = true;
      close ();
    });
    // determine which battle have been prepared
    int lastPreparedBattle = playerdata.getLastBattlePrepared ();
    std::cerr << "lastPreparedBattle: " << lastPreparedBattle << std::endl;
    // determine if the last battle has been fought
    int chapter = lastPreparedBattle / 2;
    std::cerr << "chapter: " << chapter << std::endl;
    auto        tdata = playerdata.getChapter (chapter, lastPreparedBattle % 2);
    std::string charName     = campaign.getCharacter (tdata.charID);
    std::string scenarioName = campaign.getScenario (tdata.sceneID);
    std::string cityName     = campaign.getCity (tdata.cityID);
    int         tryID = tdata.tries[0] == ufsct::chapter1::NotFought ? 0 : 1;
    std::cerr << "chapter: " << chapter
              << " lastPreparedBattle: " << lastPreparedBattle << std::endl;

    std::string battlescore = "0";

    Component battreRes = Input (&battlescore);
    battreRes |= CatchEvent ([&] (Event event) {
      return event.is_character () && !std::isdigit (event.character ()[0]);
    });

    auto layout =
      Container::Horizontal ({battreRes, winButton, loseButton, backButton});

    screen.Loop (Renderer (layout, [&] {
      return mainGrid (hbox (
        {filler (),
         hbox (
           vbox (
             window (text (scenarioName), text (cityName)),
             border (text (charName)), //
             hbox (
               window (text ("try"), text (std::to_string (1 + tryID))),
               window (text ("difficulty"), battreRes->Render ()) | flex)),
           vbox (
             winButton->Render (), loseButton->Render (),
             backButton->Render ())),
         filler ()}));
    }));

    if (goBack) {
      return navigation::ufsMain;
    }
    if (next) {
      int difficulty = 0;
      if (lost) {
        difficulty = ufsct::chapter1::Fail;
      } else {
        difficulty = std::stoi (battlescore);
      }
      // check wich battle needs to be fought

      playerdata.setTry (chapter, lastPreparedBattle % 2, tryID, difficulty);
      playerdata.save (file);
      return navigation::ufsBattle;
    }
    return navigation::ufsBattle;
  }

  template <int chapter>
  navigation battleChapterMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    std::cerr << "battleChapter" << chapter + 1 << "Menu" << std::endl;
    bool goBack = false;
    // in case of fisrt chapter the lamba of che checkob is never explored
    bool ready      = chapter == 0;
    auto close      = screen.ExitLoopClosure ();
    auto backButton = Button ("Exit", [&] {
      goBack = true;
      close ();
    });
    int  start      = 0;
    if (playerdata.getChapter (chapter, 0).charID != -1) {
      start = 2;
      std::cerr << "second battle" << std::endl;
    }
    int chosenID = -1;

    if (start == 0) {
      constexpr int      base   = 4 * chapter;
      std::array<int, 4> charID = {base, 1 + base, 2 + base, 3 + base};
      std::shuffle (charID.begin (), charID.end (), rng);
      std::array<int, 4> sceneID = {base, 1 + base, 2 + base, 3 + base};
      std::shuffle (sceneID.begin (), sceneID.end (), rng);
      if constexpr (chapter == 0) {
        std::array<int, 4> citiesID = {0, 1, 2, 3};
        std::shuffle (citiesID.begin (), citiesID.end (), rng);
        playerdata.setChapter<chapter, 4> (citiesID, charID, sceneID);
      } else {
        constexpr int      cityBase = 5 * chapter - 1; //=4 + 5 * (chapter-1);
        std::array<int, 5> citiesID = {
          cityBase, cityBase + 1, cityBase + 2, cityBase + 3, cityBase + 4};
        std::shuffle (citiesID.begin (), citiesID.end (), rng);
        playerdata.setChapter<chapter, 5> (citiesID, charID, sceneID);
      }
    }
    Components setbttn (2);
    Elements   battleChoice;
    for (int i = 0; i < 2; i++) {
      const int mychoice = i + start;
      std::cerr << i << "(" << mychoice << ")" << std::endl;
      setbttn[i] = Maybe (
        Button (
          "Set " + std::to_string (i + 1),
          [&chosenID, &close, mychoice] {
            chosenID = mychoice;
            close ();
          }),
        &ready);
      battleChoice.push_back (window (
        text ("Set " + std::to_string (i + 1)),
        vbox (
          window (
            text ("character"),
            text (campaign.getCharacter (
              playerdata.getRandomCharacterID (chapter, mychoice)))),
          window (
            text ("scenario"),
            text (campaign.getScenario (
              playerdata.getRandomScenarioID (chapter, mychoice)))),
          window (
            text ("city"),           //
            text (campaign.getCity ( //
              playerdata.getRandomCityID (chapter, mychoice)))))));
    }

    auto           choices = Container::Vertical ({Container::Horizontal (
      {Renderer ([&] { return battleChoice[0]; }),
                 Renderer ([&] { return battleChoice[1]; })})});
    CheckboxOption elitesch;
    auto           elitesChoiceIDs = playerdata.getPossibleElites (chapter);
    std::array<bool, 2 * chapter> elitesChoice;
    elitesch.on_change = [&] {
      int choices =
        std::count (elitesChoice.begin (), elitesChoice.end (), true);
      ready = choices == chapter;
    };
    if constexpr (chapter > 0) {
      auto elites = Container::Vertical ({});
      for (int i = 0; i < elitesChoiceIDs.size (); i++) {
        std::cerr << "\tElite:" << elitesChoiceIDs[i] << std::endl;
        elitesChoice[i] = false;
        elites->Add (Checkbox (
          campaign.getCharacter (elitesChoiceIDs[i]), &elitesChoice[i],
          elitesch));
      }
      choices->Add (elites);
    }

    auto layout = Container::Horizontal (
      {choices,

       Container::Vertical (
         {Renderer ([&] { return filler (); }), setbttn[0], setbttn[1],
          backButton})});

    screen.Loop (Renderer (layout, [&] {
      return mainGrid (vbox (
        hbox (
          {filler (), text ("Chapter " + std::to_string (chapter + 1)),
           text ("Battle" + std::to_string (start / 2 + 1)), filler ()}),
        hbox ({filler (), layout->Render (), filler ()})));
    }));

    if (goBack) {
      return navigation::ufsMain;
    }
    // so that becames 1 or 0
    start /= 2;
    std::cerr << "chosenID  : " << chosenID << std::endl;
    std::cerr << "charID    : "
              << playerdata.getRandomCharacterID (chapter, chosenID) << "\n";
    std::cerr << "scenarioID: "
              << playerdata.getRandomScenarioID (chapter, chosenID) << "\n";
    std::cerr << "cityID    : "
              << playerdata.getRandomCityID (chapter, chosenID) << "\n";
    for (int i = 0; i < chapter; i++) {
      if (elitesChoice[i]) {
        std::cerr << "eliteID   : " << elitesChoiceIDs[i] << "\n";
      }
    }
    if constexpr (chapter == 0) {
      playerdata.getFirstChapter (start).charID =
        playerdata.getRandomCharacterID (chapter, chosenID);
      playerdata.getFirstChapter (start).sceneID =
        playerdata.getRandomScenarioID (chapter, chosenID);
      playerdata.getFirstChapter (start).cityID =
        playerdata.getRandomCityID (chapter, chosenID);
    } else if constexpr (chapter == 1) {
      playerdata.getSecondChapter (start).charID =
        playerdata.getRandomCharacterID (chapter, chosenID);
      playerdata.getSecondChapter (start).sceneID =
        playerdata.getRandomScenarioID (chapter, chosenID);
      playerdata.getSecondChapter (start).cityID =
        playerdata.getRandomCityID (chapter, chosenID);
      for (int i = 0; i < 2; i++) {
        std::cerr << i << ":" << elitesChoice[i] << "->" << elitesChoiceIDs[i]
                  << std::endl;
        if (elitesChoice[i]) {
          playerdata.getSecondChapter (start).elitecharID = elitesChoiceIDs[i];
        }
      }
    } else if constexpr (chapter == 2) {
      playerdata.getThirdChapter (start).charID =
        playerdata.getRandomCharacterID (chapter, chosenID);
      playerdata.getThirdChapter (start).sceneID =
        playerdata.getRandomScenarioID (chapter, chosenID);
      playerdata.getThirdChapter (start).cityID =
        playerdata.getRandomCityID (chapter, chosenID);
      int i = 0;
      for (i = 0; i < 4; i++) {
        std::cerr << i << ":" << elitesChoice[i] << "->" << elitesChoiceIDs[i]
                  << std::endl;
        if (elitesChoice[i]) {
          playerdata.getThirdChapter (start).elitecharID = elitesChoiceIDs[i];
          break;
        }
      }
      ++i;
      for (; i < 4; i++) {
        std::cerr << i << ":" << elitesChoice[i] << "->" << elitesChoiceIDs[i]
                  << std::endl;
        if (elitesChoice[i]) {
          playerdata.getThirdChapter (start).elitecharID2 = elitesChoiceIDs[i];
          break;
        }
      }
    }
    {
      auto ch = playerdata.getChapter (chapter, start);
      std::cerr << "chapter   : " << chapter << ", (" << start << ")\n";
      std::cerr << "cityID    : " << ch.cityID << "\n";
      std::cerr << "charID    : " << ch.charID << "\n";
      std::cerr << "sceneID   : " << ch.sceneID << "\n";
      std::cerr << "elitecharID: " << ch.elitecharID << "\n";
      std::cerr << "elitecharID2: " << ch.elitecharID2 << "\n";
    }
    playerdata.save (file);
    return navigation::ufsBattle;
  }

  navigation loadMenu (ftxui::ScreenInteractive &screen) {
    using namespace ftxui;
    bool                     goBack      = false;
    bool                     readyToLoad = false;
    bool                     fileSet     = false;
    int                      selection2  = 0;
    ftxui::MenuOption        option;
    auto                     close = screen.ExitLoopClosure ();
    std::vector<std::string> FileAndDirs;
    std::filesystem::path    path   = std::filesystem::current_path ();
    std::string              visual = path.string ();

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
              file   = (path / selected).string ();

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
      readyToLoad   = false;
      if (selected == "..") {
        path   = path.parent_path ();
        visual = path.string ();
      } else {
        if (std::filesystem::is_directory (path / selected)) {
          path /= selected;
          visual = path.string ();
        } else {
          visual      = (path / selected).string ();
          readyToLoad = true;
        }
      }
      close ();
    };

    option.on_change = [&] {
      auto selected = FileAndDirs[selection2];
      readyToLoad   = !std::filesystem::is_directory (path / selected);
    };

    auto dirMenu = Menu (&FileAndDirs, &selection2, option);
    auto layout  = Container::Horizontal (
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
      campaignName = playerdata.getCampaignName ();
      return navigation::ufsBattle;
    }
    return navigation::ufsLoad;
  }
};

int main (int, char **) {
  using namespace ftxui;
  auto                     screen = ScreenInteractive::Fullscreen ();
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

// using namespace ftxui;

// ButtonOption Style () {
//   auto option = ButtonOption::Animated ();
//   option.transform = [] (const EntryState &s) {
//     auto element = text (s.label);
//     if (s.focused) {
//       element |= bold;
//     }
//     return element | center | borderEmpty | flex;
//   };
//   return option;
// }

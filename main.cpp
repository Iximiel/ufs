#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <string>
#include <vector>

int main (void) {
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen ();
  auto title = Renderer ([] {
    return hbox (
      {filler (), text ("Under Falling Skies campaign tracker"), filler ()});
  });
  const std::vector<std::string> menu_entries = {
    "New Game", "Load Game", "Exit"};
  int selected = 0;
  MenuOption option;
  option.on_enter = screen.ExitLoopClosure ();
  auto menu = Menu (&menu_entries, &selected, option);
  auto centerMenu =
    Renderer ([&] { return hbox ({filler (), menu->Render (), filler ()}); });

  auto layout = Container::Vertical ({title, centerMenu});

  auto window = Renderer (layout, [&] {
    return vbox (
      {title->Render (), separator (), centerMenu->Render (), filler (),
       separator (), text ("Version 0.1")});
  });
  screen.Loop (window);
  return EXIT_SUCCESS;
}
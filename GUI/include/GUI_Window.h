#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "TestSystem.h"
#include <gtkmm.h>
#include <string>

namespace GUI {

//____________________________________Test
//Window________________________________________

class TestWindow : public Gtk::Dialog {
public:
  TestWindow(const TestSystem::TestPaper &Test);
  std::vector<unsigned> getFinalResults() const { return FinalResults; }
  Gtk::Grid &getGrid() { return Grid; }

  // Signal handlers
  void handleButtonAnswer(int i, int j);
  void handleButtonClose();

protected:
  // Child widgets
  Gtk::ScrolledWindow ScrolledWindow;
  Gtk::Grid Grid;
  Gtk::Button ButtonClose;
  std::vector<unsigned> FinalResults;
};

} // namespace GUI

#endif // GUI_WINDOW_H

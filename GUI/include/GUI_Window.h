#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <gtkmm.h>
#include <string>
#include "TestSystem.h"

class ExampleWindow : public Gtk::Dialog
{
public:
  ExampleWindow(const TestSystem::TestPaper &Test);
  virtual ~ExampleWindow();
  std::string getResults() const;
  std::vector<unsigned> getFinalResults() const;
  void addTest(const TestSystem::TestPaper &Test);

protected:
  //Signal handlers:
  void on_button_clicked(int i, int j);
  void on_button_close();

  //Child widgets:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::Grid m_Grid;
  Gtk::Button m_Button_Close;
  std::vector<unsigned> FinalResults;
};

#endif //GUI_WINDOW_H

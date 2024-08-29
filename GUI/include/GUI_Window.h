#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <gtkmm.h>
#include <string>

class ExampleWindow : public Gtk::Dialog
{
public:
  ExampleWindow();
  virtual ~ExampleWindow();
  std::string getResults() const;

protected:
  //Signal handlers:
  void on_button_clicked(Glib::ustring data);
  void on_button_close();

  //Child widgets:
  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::Grid m_Grid;
  Gtk::Button m_Button_Close;
  int final_results[5];
};

#endif //GUI_WINDOW_H

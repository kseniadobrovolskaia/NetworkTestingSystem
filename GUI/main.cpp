#include "GUI_Window.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  ExampleWindow window({TestSystem::TestPaper()});

  return app->run(window);
}


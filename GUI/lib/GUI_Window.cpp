#include "GUI_Window.h"
#include <cstdio>
#include <iostream>

ExampleWindow::ExampleWindow()
: m_Button_Close("Send answers!") {
  for (int i = 0; i < 5; ++i)
    final_results[i] = 0; 
  set_title("Test!");
  set_border_width(0);
  set_size_request(1500, 600);

  m_ScrolledWindow.set_border_width(10);

  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);

  get_content_area()->pack_start(m_ScrolledWindow);

  m_Grid.set_row_spacing(10);
  m_Grid.set_column_spacing(10);

  m_ScrolledWindow.add(m_Grid);

  int count_answers[] = {4, 4, 4, 4, 4};
  int count_quations = 5;
  int i = 0;
  const char *Strings[] = {"Какое животное имеет второе название — кугуар?",
                           "Оцелот",  
                           "Леопард",
                           "Пума", 
                           "Пантера", 
                           
                           "На что кладут руку члены английского общества лысых во время присяги?",
                           "Баскетбольный мяч",
                           "Бильярдный шар",
                           "Апельсин",
                           "Кокосовый орех",
                           "Какой химический элемент назван в честь злого подземного гнома?",
                           "Гафний",
                           "Кобальт",
                           "Бериллий",
                           "Теллур",
                           
                           "В честь какого растения область Фриули-Венеция-Джулия в Италии ежегодно проводит трёхмесячный фестиваль?",
                           "Лук",
                           "Фасоль",
                           "Спаржа",
                           "Артишок",
                           "Что Иван Ефремов в романе «Лезвие бритвы» назвал наивысшей степенью целесообразности?",
                           "Красоту",
                           "Смерть",
                           "Мудрость",
                           "Свободу"
  };


  int NumStr = 0;
  for(int j = 1; j < count_quations * 2; j += 2) {
     Gtk::TextView *pText = Gtk::manage(new Gtk::TextView());
     Glib::RefPtr<Gtk::TextBuffer> refTextBuffer = pText->get_buffer();
     Gtk::TextBuffer::iterator iter = refTextBuffer->get_iter_at_offset(0);
     char text[256];
     sprintf(text, "\n%d. %s", j / 2 + 1, Strings[NumStr]);
     NumStr++;
     refTextBuffer->insert(iter,
     text);
     m_Grid.attach(*pText, 0, j - 1, 1, 1);

     Gtk::RadioButton::Group group;
     for (int i = 0; i < count_answers[j / 2]; ++i) {
       char buffer[32];
       sprintf(buffer, "(%d,%d) %s", i, j / 2, Strings[NumStr]);
       NumStr++;
       Gtk::Button* pButton = Gtk::manage(new Gtk::RadioButton(group, buffer));
       pButton->signal_clicked().connect(sigc::bind<-1, Glib::ustring>(
             sigc::mem_fun(*this, &ExampleWindow::on_button_clicked), buffer));
       m_Grid.attach(*pButton, i, j, 1, 1);
     }
  }
  
  m_Button_Close.signal_clicked().connect( sigc::mem_fun(*this,
              &ExampleWindow::on_button_close));

  m_Button_Close.set_can_default();

  Gtk::Box* pBox = get_action_area();
  
  if(pBox)
    pBox->pack_start(m_Button_Close);

  m_Button_Close.grab_default();

  show_all_children();
}

void ExampleWindow::on_button_close() {
  hide();
}

ExampleWindow::~ExampleWindow() {
}

void ExampleWindow::on_button_clicked(Glib::ustring data) {
  int i = data.c_str()[1] - '0';
  int j = data.c_str()[3] - '0';
  final_results[j] = i;
}

std::string ExampleWindow::getResults() const {
  std::string str;
  str.reserve(10);
  for (int i = 0; i < 4; ++i) {
    str[i * 2] = std::to_string(final_results[i] + 1)[0];
    str[i * 2 + 1] = ' ';
  }
  str[8] = std::to_string(final_results[4] + 1)[0];
  str[9] = '\0';
  return str;
}

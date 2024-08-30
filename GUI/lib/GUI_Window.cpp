#include "GUI_Window.h"
#include <cstdio>
#include <iostream>



void ExampleWindow::addTest(const TestSystem::TestPaper &Test) {


}


ExampleWindow::ExampleWindow(const TestSystem::TestPaper &Test)
: m_Button_Close("Send answers!") {



  auto Questions = Test.questions();
  auto CountQuestions = Questions.size();

  FinalResults.resize(CountQuestions);
  for (int i = 0; i < CountQuestions; ++i)
    FinalResults[i] = 0; 
  set_title(Test.name());
  set_border_width(0);
  set_size_request(1500, 600);

  m_ScrolledWindow.set_border_width(10);

  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);

  get_content_area()->pack_start(m_ScrolledWindow);

  m_Grid.set_row_spacing(10);
  m_Grid.set_column_spacing(10);

  m_ScrolledWindow.add(m_Grid);

  std::vector<unsigned> CountAnswers;
  CountAnswers.reserve(CountQuestions);
  for (const auto &Q : Questions)
    CountAnswers.push_back(Q.answers().size());
 
  int NumStr = 0;
  for(int j = 1; j < CountQuestions * 2; j += 2) {
     Gtk::TextView *pText = Gtk::manage(new Gtk::TextView());
     Glib::RefPtr<Gtk::TextBuffer> refTextBuffer = pText->get_buffer();
     Gtk::TextBuffer::iterator iter = refTextBuffer->get_iter_at_offset(0);
     std::ostringstream text;
     text << "\n" << j / 2 + 1 << ". " << Questions[j / 2].formulation();
     refTextBuffer->insert(iter, text.str());
     m_Grid.attach(*pText, 0, j - 1, 1, 1);

     Gtk::RadioButton::Group group;
     
     for (int i = 0; i < CountAnswers[j / 2]; ++i) {
       std::ostringstream ButtonName;
       ButtonName << Questions[j / 2].answers()[i];
       Gtk::Button* pButton = Gtk::manage(new Gtk::RadioButton(group, ButtonName.str()));
       pButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &ExampleWindow::on_button_clicked), i, j / 2));
       m_Grid.attach(*pButton, i, j, 1, 1);
     }
  }
  
  m_Button_Close.signal_clicked().connect(sigc::mem_fun(*this, &ExampleWindow::on_button_close));

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

void ExampleWindow::on_button_clicked(int i, int j) {
  FinalResults[j] = i;
}

std::vector<unsigned> ExampleWindow::getFinalResults() const {
  return FinalResults;
}

std::string ExampleWindow::getResults() const {
  std::string str;
  str.reserve(FinalResults.size() * 2 - 1);
  std::cout << "\n\nResults size: " << FinalResults.size() << "\n\n";
  for (int i = 0; i < FinalResults.size() - 1; ++i) {
    str[i * 2] = std::to_string(FinalResults[i] + 1)[0];
    str[i * 2 + 1] = ' ';
  }
  str[(FinalResults.size() - 1) * 2] = std::to_string(FinalResults.back() + 1)[0];
  //str[FinalResults.size() * 2] = '\0';
  return str;
}

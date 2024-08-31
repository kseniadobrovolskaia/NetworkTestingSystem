#include "GUI_Window.h"
#include <cstdio>
#include <iostream>

#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 600
#define BORDER_WIDTH 10

namespace GUI {

static void addButtonToGroup(unsigned NumQuestion, unsigned NumAnswer,
                             const std::string &Answer,
                             Gtk::RadioButton::Group &Group, Gtk::Box *Box,
                             TestWindow *Window) {
  std::ostringstream ButtonName;
  ButtonName << Answer;
  Gtk::Button *pButton =
      Gtk::manage(new Gtk::RadioButton(Group, ButtonName.str()));
  pButton->signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*Window, &TestWindow::handleButtonAnswer),
                 NumAnswer, NumQuestion));
  Box->pack_start(*pButton, Gtk::PACK_SHRINK);
}

static void addFrameWithQuestion(unsigned NumQuestion,
                                 const TestSystem::OneAnswerQuestion &Question,
                                 TestWindow *Window) {
  std::ostringstream QuestFormulation;
  QuestFormulation << "\n" << NumQuestion + 1 << ". " << Question.formulation();

  Gtk::Box *Box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10));
  Gtk::Frame *Frm = Gtk::manage(new Gtk::Frame(QuestFormulation.str()));
  Frm->set_shadow_type(Gtk::SHADOW_ETCHED_OUT);
  Frm->set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_END);
  Frm->add(*Box);
  Window->getGrid().attach(*Frm, 0, NumQuestion, 1, 1);

  Gtk::RadioButton::Group Group;
  std::for_each(Question.answers().begin(), Question.answers().end(),
                [NumAnswer = 0, &Group, NumQuestion, Box,
                 Window](const auto &Answer) mutable {
                  addButtonToGroup(NumQuestion, NumAnswer++, Answer, Group, Box,
                                   Window);
                });
}

//____________________________________Test_Window________________________________________

TestWindow::TestWindow(const TestSystem::TestPaper &Test)
    : ButtonClose("Send answers!") {
  auto Questions = Test.questions();

  // Number of answers coincides with the number of questions, since all
  // questions have one answer option
  FinalResults.resize(Questions.size(), 0);

  set_title(Test.name());
  set_border_width(BORDER_WIDTH);
  set_size_request(WINDOW_WIDTH, WINDOW_HEIGHT);

  ScrolledWindow.set_border_width(BORDER_WIDTH);
  ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);

  // In content area place Scrolled Window with questions
  get_content_area()->pack_start(ScrolledWindow);

  Grid.set_row_spacing(BORDER_WIDTH);
  Grid.set_column_spacing(BORDER_WIDTH);
  Grid.set_column_homogeneous(200);

  ScrolledWindow.add(Grid);

  std::for_each(Questions.begin(), Questions.end(),
                [NumQuestion = 0, this](const auto &Question) mutable {
                  addFrameWithQuestion(NumQuestion++, Question, this);
                });

  ButtonClose.signal_clicked().connect(
      sigc::mem_fun(*this, &TestWindow::handleButtonClose));
  ButtonClose.set_can_default();

  // In action area place one Close Button
  get_action_area()->pack_start(ButtonClose);
  ButtonClose.grab_default();

  show_all_children();
}

//__________________________________Button_handlers______________________________________

void TestWindow::handleButtonClose() { hide(); }

void TestWindow::handleButtonAnswer(int NumAnswer, int NumQuestion) {
  FinalResults[NumQuestion] = NumAnswer;
}

} // namespace GUI

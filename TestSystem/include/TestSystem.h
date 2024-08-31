#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "json.hpp"

namespace TestSystem {

//____________________________________Question___________________________________________

class Question {
protected:
public:
  Question() = default;
  virtual ~Question() {}
  virtual void print() const = 0;
  virtual const std::string &formulation() const = 0;
  virtual std::string &formulation() = 0;

  // To work with json parsing
  virtual void toJson(nlohmann::json &J) const = 0;
  virtual void fromJson(const nlohmann::json &J) = 0;
};

void to_json(nlohmann::json &J, const Question &Q);
void from_json(const nlohmann::json &J, Question &Q);

//_______________________________One Answer
//Question_____________________________________

class OneAnswerQuestion : public Question {
  std::vector<std::string> Answers;
  std::string CorrectAnswer;
  std::string Formulation = "";

public:
  OneAnswerQuestion() = default;
  // OneAnswerQuestion(const nlohmann::json &J) { fromJson(J); }
  OneAnswerQuestion(const std::vector<std::string> &Answers,
                    const std::string &CorrectAnswer, std::string Formulation)
      : Answers(Answers), CorrectAnswer(CorrectAnswer),
        Formulation(Formulation){};
  OneAnswerQuestion(const OneAnswerQuestion &Q)
      : Answers(Q.Answers), CorrectAnswer(Q.CorrectAnswer),
        Formulation(Q.Formulation){};
  OneAnswerQuestion(const std::string &F) : Formulation(F){};
  const std::string &formulation() const override { return Formulation; }
  std::string &formulation() override { return Formulation; }
  const std::string &correctAnswer() const { return CorrectAnswer; }
  std::string &correctAnswer() { return CorrectAnswer; }
  const std::vector<std::string> &answers() const { return Answers; }
  std::vector<std::string> &answers() { return Answers; }

  void print() const override {
    std::cout << "Question with one correct answer: \n";
    std::cout << "\n    Formulation: " << Formulation << "\n";
    std::cout << "\n    Answers: \n";
    std::for_each(Answers.begin(), Answers.end(),
                  [Idx = 0](const auto &A) mutable {
                    std::cout << "      " << ++Idx << ". " << A << "\n";
                  });
    std::cout << "\n    Correct answer: " << CorrectAnswer << "\n";
  }

  // To work with json parsing
  void toJson(nlohmann::json &J) const override {
    J = nlohmann::json{{"Formulation", Formulation},
                       {"Answers", Answers},
                       {"CorrectAnswer", CorrectAnswer}};
  }

  void fromJson(const nlohmann::json &J) override {
    J.at("Formulation").get_to(Formulation);
    J.at("Answers").get_to(Answers);
    J.at("CorrectAnswer").get_to(CorrectAnswer);
  }
};

//___________________________________Test
//Paper__________________________________________

class TestPaper {
  std::vector<OneAnswerQuestion> Questions;
  std::string Name = "";

public:
  TestPaper() = default;
  TestPaper(const std::string &Name) : Name(Name) {}
  const std::string &name() const { return Name; }
  const std::vector<OneAnswerQuestion> &questions() const { return Questions; }
  std::vector<OneAnswerQuestion> &questions() { return Questions; }
  size_t countQuestions() const { return Questions.size(); }

  // void addQuestion(const OneAnswerQuestion &Q) {
  // Questions.emplace_back(std::make_unique(Q)); }

  void print() const {
    std::cout << "Test Name : " << Name << "\n";
    std::cout << "Questions :\n";
    std::for_each(Questions.begin(), Questions.end(),
                  [Idx = 0](const auto &Q) mutable {
                    std::cout << "  " << ++Idx << ". ";
                    Q.print();
                    std::cout << "\n";
                  });
  }

  // To work with json parsing

  void toJson(nlohmann::json &J) const {
    J["TestName"] = Name;
    J["Questions"] = Questions;
  }

  void fromJson(const nlohmann::json &J) {
    J.at("TestName").get_to(Name);
    J.at("Questions").get_to(Questions);
  }
};

void to_json(nlohmann::json &J, const TestPaper &T);
void from_json(const nlohmann::json &J, TestPaper &T);

//_____________________________________Answer____________________________________________

class Answer {
  std::string Ans;

public:
  Answer(const std::string &Ans) : Ans(Ans){};
};

//__________________________________Answer
//Paper_________________________________________

class AnswerPaper {
  std::vector<Answer> Answers;

public:
  AnswerPaper(const std::vector<Answer> Answers) : Answers(Answers){};
};

} // namespace TestSystem

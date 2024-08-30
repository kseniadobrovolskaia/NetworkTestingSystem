#include "Server.hpp"

void failWithError(const char *Message) {
    perror(Message);
    exit(1);
}

int calculateMark(std::vector<unsigned> final_results, std::vector<unsigned> right_answers) {
  int Mark = final_results.size();
  for (int i = 0; i < final_results.size(); ++i) {
    if (final_results[i] != right_answers[i])
      --Mark;
  }
  return Mark * 5 / final_results.size();
}       

TestSystem::OneAnswerQuestion getQuestion(unsigned NumQuestion) {
  TestSystem::OneAnswerQuestion Q;
  std::cout << "Введите вопрос номер " << NumQuestion << ":\n";
  std::cin >> Q.formulation();
  std::cout << "Введите количество предлагаемых ответов:\n";
  int CountAnswers;
  std::cin >> CountAnswers;
  std::vector<std::string> Answers(CountAnswers);
  for (int NumAns = 0; NumAns < CountAnswers; ++NumAns) {
    std::cout << "Введите вариант ответа номер " << NumAns + 1 << ":\n";
    std::cin >> Answers[NumAns];
  }
  std::cout << "Введите номер правильного ответа:\n";
  int NumCorrectAns;
  std::cin >> NumCorrectAns;
  Q.correctAnswer() = Answers[NumCorrectAns - 1];
  Q.answers() = Answers;

  std::cout << "Вопрос номер " << NumQuestion << " добавлен.\n\n";

  return Q;
}

static void runTest(TestSystem::TestPaper &Test) {
  // Delete correct answers
  std::vector<unsigned> right_answers;
  right_answers.reserve(Test.questions().size());

  for (auto &Q : Test.questions()) {
    auto Position = std::distance(Q.answers().begin(), find(Q.answers().begin(), Q.answers().end(), Q.correctAnswer()));

    right_answers.push_back(Position);
    Q.correctAnswer() = "";
  }


  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  
  std::cout << "Введите номер порта:\n";
  std::cin >> portno;
  std::cout << "Тест запущен!\n";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    failWithError("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    failWithError("ERROR on binding");
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, 
              (struct sockaddr *) &cli_addr, 
              &clilen);
  if (newsockfd < 0) 
    failWithError("ERROR on accept");

 
  nlohmann::json newJson = Test;
  to_json(newJson, Test);
  std::string str = newJson.dump();

  // Send test without right answers
  n = write(newsockfd, str.c_str(), str.size());
  if (n < 0) 
    failWithError("ERROR writing to socket");
  
  // Read Answer
  std::vector<unsigned> final_results(right_answers.size());
  n = read(newsockfd,final_results.data(), sizeof(right_answers));
  if (n < 0) 
    failWithError("ERROR reading from socket");
  std::cout << "Here is the answer: ";
  for (auto A : final_results)
    std::cout << A << " ";
  std::cout << "\n";
  
  // Send right answers
  n = write(newsockfd, right_answers.data(), sizeof(right_answers));
  if (n < 0) 
    failWithError("ERROR writing to socket");
  
  // Send mark
  n = write(newsockfd, std::to_string(calculateMark(final_results, right_answers)).c_str(), 1);
  if (n < 0) 
    failWithError("ERROR writing to socket");

  close(newsockfd);
  close(sockfd);
}


void createAction() {
  std::string Name;
  std::cout << "Введите название теста:\n";
  std::cin >> Name;
  TestSystem::TestPaper Test(Name);
  std::vector<TestSystem::OneAnswerQuestion> Questions;
  unsigned NumQuestion = 1;
  bool isMoreQuestions = true;

  while (isMoreQuestions) {
    Questions.push_back(getQuestion(NumQuestion++));
    std::cout << "Какое действие сделать? (Напишите число)\n"
                 "  1 - Добавить ещё вопрос\n"
                 "  2 - Завершить создание теста и запустить его\n";
    int Ans;
    std::cin >> Ans;
    switch (Ans) {
      case 1:
        continue;
      case 2:
        isMoreQuestions = false;
        break;
      default:
        failWithError("Incorrect answer");
    }
  }

  Test.questions() = Questions;
  
  runTest(Test);
}

void saveAction() {
  std::string Name;
  std::cout << "Введите название теста:\n";
  std::cin >> Name;
  TestSystem::TestPaper Test(Name);
  std::vector<TestSystem::OneAnswerQuestion> Questions;
  unsigned NumQuestion = 1;
  bool isMoreQuestions = true;

  while (isMoreQuestions) {
    Questions.push_back(getQuestion(NumQuestion++));
    std::cout << "Какое действие сделать? (Напишите число)\n"
                 "  1 - Добавить ещё вопрос\n"
                 "  2 - Завершить создание теста и сохранить его\n";
    int Ans;
    std::cin >> Ans;
    switch (Ans) {
      case 1:
        continue;
      case 2:
        isMoreQuestions = false;
        break;
      default:
        failWithError("Incorrect answer");
    }
  }

  Test.questions() = Questions;

  std::string FileName;
  std::cout << "Введите название файла для сохранения:\n";
  std::cin >> FileName;

  std::ofstream o(FileName);
  nlohmann::json Json;
  to_json(Json, Test);
  o << std::setw(4) << Json << std::endl;
  std::cout << "Тест сохранён в файле " << FileName << ".\n";
  
}


void runFromFileAction() {
  std::cout << "Введите путь до файла с тестом:\n";
  std::string FilePath;
  std::cin >> FilePath;

  std::ifstream f(FilePath);
  nlohmann::json data = nlohmann::json::parse(f);

  auto Test = data.template get<TestSystem::TestPaper>();
  runTest(Test);
}

void chooseAction() {
  std::cout << "Какое действие сделать? (Напишите число)\n"
         "  1 - Создать тест и запустить его\n"
         "  2 - Создать тест и сохранить в файле\n"
         "  3 - Запустить тест из файлa\n";
  int Ans;
  std::cin >> Ans;
  switch (Ans) {
    case 1: 
      createAction();
      break;
    case 2:
      saveAction();
      break;
    case 3:
      runFromFileAction();
      break;
    default:
      failWithError("Incorrect answer");

  }
}


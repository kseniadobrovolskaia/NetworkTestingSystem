#include "Server.hpp"
#include <signal.h>

namespace TestServer {

void failWithError(const std::string &Msg) {
  throw std::logic_error("\n\n" + Msg);
}

int calculateMark(std::vector<unsigned> ClientAnswers,
                  std::vector<unsigned> CorrectAnswers) {
  constexpr unsigned BestMark = 5;
  unsigned NumOfCorrect = 0;

  for (auto NumAns = 0; NumAns < ClientAnswers.size(); ++NumAns)
    NumOfCorrect += (ClientAnswers.at(NumAns) == CorrectAnswers.at(NumAns));

  return NumOfCorrect * BestMark / ClientAnswers.size();
}

// To shut down the server.
// Set in the SIG_INT signal handler.
volatile bool Exit = false;

static void runTest(TestSystem::TestPaper &Test) {
  // This is a reasonable value for the number of clients
  // waiting for a response from the server on many platforms.
  constexpr unsigned MaxClientsQueue = 5;
  // Delete correct answers and save them in vector
  std::vector<unsigned> CorrectAnswers;
  CorrectAnswers.reserve(Test.questions().size());

  for (auto &Q : Test.questions()) {
    auto Position = std::distance(
        Q.answers().begin(),
        find(Q.answers().begin(), Q.answers().end(), Q.correctAnswer()));

    CorrectAnswers.push_back(Position);
    Q.correctAnswer() = "";
  }
  // Prepare test in string format
  nlohmann::json newJson = Test;
  to_json(newJson, Test);
  std::string TestStr = newJson.dump();

  // Socket to accept client's connections
  int sockfd, newsockfd, portno, pid;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  std::cout << "Введите номер порта:\n";
  std::cin >> portno;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    failWithError("ERROR opening socket");
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    failWithError("ERROR on binding");
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  std::cout << "Тест запущен!\n";
  // To kill zombie child processes
  signal(SIGCHLD, SIG_IGN);
  while (!Exit) {
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      failWithError("ERROR on accept");
    pid = fork();
    if (pid < 0)
      failWithError("ERROR on fork");
    if (pid == 0) {
      close(sockfd);
      serveTheClient(newsockfd, TestStr, CorrectAnswers);
      exit(EXIT_SUCCESS);
    } else
      close(newsockfd);
  }

  close(sockfd);
}

static void writeWithCheck(int fd, const void *buf, size_t count,
                           const std::string &Msg) {
  if (write(fd, buf, count) < 0)
    failWithError(Msg);
}

static void readWithCheck(int fd, void *buf, size_t count,
                          const std::string &Msg) {
  if (read(fd, buf, count) < 0)
    failWithError(Msg);
}

// There is a function for each connection. It handles all
// communication once a connnection has been established.
void serveTheClient(int sockfd, const std::string &TestStr,
                    const std::vector<unsigned> &CorrectAnswers) {
  // Send test without right answers
  writeWithCheck(sockfd, TestStr.c_str(), TestStr.size(),
                 "ERROR writing to socket");

  // Read Answer
  std::vector<unsigned> ClientAnswers(CorrectAnswers.size());
  readWithCheck(sockfd, ClientAnswers.data(), sizeof(CorrectAnswers),
                "ERROR reading from socket");

  // Send right answers
  writeWithCheck(sockfd, CorrectAnswers.data(), sizeof(CorrectAnswers),
                 "ERROR writing to socket");

  // Send mark
  writeWithCheck(
      sockfd,
      std::to_string(calculateMark(ClientAnswers, CorrectAnswers)).c_str(), 1,
      "ERROR writing to socket");
}

static TestSystem::OneAnswerQuestion getQuestion(unsigned NumQuestion) {
  TestSystem::OneAnswerQuestion Q;
  std::cout << "Введите вопрос номер " << NumQuestion << ":\n";
  getline(std::cin, Q.formulation());
  std::cout << "Введите количество предлагаемых ответов:\n";
  int CountAnswers;
  std::cin >> CountAnswers;
  getchar();
  std::vector<std::string> Answers(CountAnswers);
  std::generate(Answers.begin(), Answers.end(), [NumAns = 0]() mutable {
    std::cout << "Введите вариант ответа номер " << ++NumAns << ":\n";
    std::string Answer;
    getline(std::cin, Answer);
    return Answer;
  });
  Q.answers() = Answers;

  std::cout << "Введите номер правильного ответа:\n";
  int NumCorrectAns;
  std::cin >> NumCorrectAns;
  getchar();
  Q.correctAnswer() = Answers.at(NumCorrectAns - 1);

  std::cout << "Вопрос номер " << NumQuestion << " добавлен.\n\n";

  return Q;
}

static TestSystem::TestPaper getTestPaper() {
  std::cout << "Введите название теста:\n";
  std::string Name;
  getline(std::cin, Name);
  TestSystem::TestPaper Test(Name);
  unsigned NumQuestion = 0;
  bool isMoreQuestions = true;
  std::vector<TestSystem::OneAnswerQuestion> Questions;

  while (isMoreQuestions) {
    Questions.emplace_back(getQuestion(++NumQuestion));
    std::cout << "Какое действие сделать? (Напишите число)\n"
                 "  1 - Добавить ещё вопрос\n"
                 "  2 - Завершить создание теста\n";
    int Ans;
    std::cin >> Ans;
    getchar();
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
  return Test;
}

void createAction() {
  TestSystem::TestPaper Test = getTestPaper();

  runTest(Test);
}

void saveAction() {
  TestSystem::TestPaper Test = getTestPaper();

  std::string FileName;
  std::cout << "Введите название файла для сохранения:\n";
  std::cin >> FileName;

  std::ofstream File(FileName);
  nlohmann::json Json;
  to_json(Json, Test);
  File << std::setw(4) << Json << "\n";
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

void exitHandler(int Signo) {
  std::cout << "Работа сервера завершена.\n";
  Exit = true;
  exit(EXIT_SUCCESS);
}

void chooseAction() {
  // To exit
  signal(SIGINT, exitHandler);
  std::cout << "Какое действие сделать? (Напишите число)\n"
               "  1 - Создать тест и запустить его\n"
               "  2 - Создать тест и сохранить в файле\n"
               "  3 - Запустить тест из файлa\n";
  int Ans;
  std::cin >> Ans;
  getchar();
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

} // namespace TestServer

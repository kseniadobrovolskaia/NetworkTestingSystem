#include "Server.hpp"
#include <signal.h>

namespace TestServer {

void failWithError(const std::string &Msg) {
  throw std::logic_error("\n\n" + Msg);
}

int calculateMark(std::vector<unsigned> ClientAnswers,
                  std::vector<unsigned> CorrectAnswers) {
  constexpr auto BestMark = 5u;
  auto NumOfCorrect = 0u;

  for (auto NumAns = 0; NumAns < ClientAnswers.size(); ++NumAns)
    NumOfCorrect += (ClientAnswers.at(NumAns) == CorrectAnswers.at(NumAns));

  return NumOfCorrect * BestMark / ClientAnswers.size();
}

// To shut down the server.
// Set in the SIG_INT signal handler.
volatile auto Exit = false;

static void runTest(TestSystem::TestPaper &Test, unsigned Port) {
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
  nlohmann::json JsonTest = Test;
  to_json(JsonTest, Test);
  auto TestStr = JsonTest.dump();

  // Socket to accept client's connections
  auto ListenSockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (ListenSockFd < 0)
    failWithError("Error opening socket");
  // Fill Server data
  sockaddr_in ServerAddr, ClientAddr;
  bzero((char *)&ServerAddr, sizeof(ServerAddr));
  ServerAddr.sin_family = AF_INET;
  ServerAddr.sin_addr.s_addr = INADDR_ANY;
  ServerAddr.sin_port = htons(Port);

  if (bind(ListenSockFd, (sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    failWithError("Error on binding");

  // This is a reasonable value for the number of clients
  // waiting for a response from the server on many platforms.
  constexpr unsigned MaxClientsQueue = 5;
  listen(ListenSockFd, MaxClientsQueue);
  socklen_t ClientSize = sizeof(ClientAddr);

  std::cout << "Тест запущен!\n";
  // To kill zombie child processes
  signal(SIGCHLD, SIG_IGN);
  while (!Exit) {
    auto AcceptSockFd =
        accept(ListenSockFd, (sockaddr *)&ClientAddr, &ClientSize);
    if (AcceptSockFd < 0)
      failWithError("Error on accept");
    auto Pid = fork();
    if (Pid < 0)
      failWithError("Error on fork");
    if (Pid == 0) {
      close(ListenSockFd);
      serveTheClient(AcceptSockFd, TestStr, CorrectAnswers);
      exit(EXIT_SUCCESS);
    } else
      close(AcceptSockFd);
  }

  close(ListenSockFd);
}

static void writeWithCheck(int Fd, const void *Buf, size_t Count,
                           const std::string &Msg) {
  if (write(Fd, Buf, Count) < 0)
    failWithError(Msg);
}

static void readWithCheck(int Fd, void *Buf, size_t Count,
                          const std::string &Msg) {
  if (read(Fd, Buf, Count) < 0)
    failWithError(Msg);
}

// There is a function for each connection. It handles all
// communication once a connnection has been established.
void serveTheClient(int SockFd, const std::string &TestStr,
                    const std::vector<unsigned> &CorrectAnswers) {
  // Send test without right answers
  writeWithCheck(SockFd, TestStr.c_str(), TestStr.size(),
                 "Error writing to socket");

  // Read Answer
  std::vector<unsigned> ClientAnswers(CorrectAnswers.size());
  readWithCheck(SockFd, ClientAnswers.data(), sizeof(CorrectAnswers),
                "Error reading from socket");

  // Send right answers
  writeWithCheck(SockFd, CorrectAnswers.data(), sizeof(CorrectAnswers),
                 "Error writing to socket");

  // Send mark
  writeWithCheck(
      SockFd,
      std::to_string(calculateMark(ClientAnswers, CorrectAnswers)).c_str(),
      /* Size */ 1, "Error writing to socket");
}

/**
 * @brief getUnsignedFromStr - it helps get number and process incorrect values,
 *                             which are given for unsigned argument.
 */
static unsigned getUnsignedFromStr() {
  std::string GivenValue;
  std::getline(std::cin, GivenValue);
  // find sign
  // return stoi(GivenValue);
  auto FirstLetter = GivenValue.begin();
  while (isspace((unsigned char)*FirstLetter))
    ++FirstLetter;
  auto Sign = *FirstLetter;
  char *Endptr; //  Store the location where conversion stopped
  auto TryValue = strtoul(GivenValue.c_str() +
                              std::distance(GivenValue.begin(), FirstLetter),
                          &Endptr, /* base */ 10);

  if (GivenValue == Endptr)
    failWithError("Invalid number provided");
  else if (*Endptr)
    failWithError("Extra text after number");
  else if (Sign == '-' && TryValue != 0)
    failWithError("Negative number");
  return TryValue;
}

static TestSystem::OneAnswerQuestion getQuestion(unsigned NumQuestion) {
  TestSystem::OneAnswerQuestion Q;
  std::cout << "Введите вопрос номер " << NumQuestion << ":\n";
  std::getline(std::cin, Q.formulation());
  std::cout << "Введите количество предлагаемых ответов:\n";
  auto CountAnswers = getUnsignedFromStr();
  std::vector<std::string> Answers(CountAnswers);
  std::generate(Answers.begin(), Answers.end(), [NumAns = 0]() mutable {
    std::cout << "Введите вариант ответа номер " << ++NumAns << ":\n";
    std::string Answer;
    getline(std::cin, Answer);
    return Answer;
  });
  Q.answers() = Answers;

  std::cout << "Введите номер правильного ответа:\n";
  auto NumCorrectAns = getUnsignedFromStr();
  if (NumCorrectAns > Answers.size())
    failWithError("Нет такого варианта ответа");
  Q.correctAnswer() = Answers.at(NumCorrectAns - 1);

  std::cout << "Вопрос номер " << NumQuestion << " добавлен.\n\n";
  return Q;
}

static TestSystem::TestPaper getTestPaper() {
  std::cout << "Введите название теста:\n";
  std::string Name;
  getline(std::cin, Name);
  TestSystem::TestPaper Test(Name);
  auto NumQuestion = 0u;
  auto isMoreQuestions = true;
  std::vector<TestSystem::OneAnswerQuestion> Questions;
  while (isMoreQuestions) {
    Questions.emplace_back(getQuestion(++NumQuestion));
    std::cout << "Какое действие сделать? (Напишите число)\n"
                 "  1 - Добавить ещё вопрос\n"
                 "  2 - Завершить создание теста\n";
    auto Ans = getUnsignedFromStr();
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

static unsigned getPort() {
  std::cout << "Введите номер порта:\n";
  auto Port = getUnsignedFromStr();
  return Port;
}

void createAction() {
  auto Test = getTestPaper();

  runTest(Test, getPort());
}

void saveAction() {
  auto Test = getTestPaper();

  std::cout << "Введите название файла для сохранения:\n";
  std::string FileName;
  getline(std::cin, FileName);

  std::ofstream OutFile(FileName);
  nlohmann::json Json;
  to_json(Json, Test);
  OutFile << std::setw(4) << Json << "\n";
  std::cout << "Тест сохранён в файле " << FileName << ".\n";
}

void exitHandler(int Signo) {
  std::cout << "\nРабота сервера завершена.\n";
  Exit = true;
  exit(EXIT_SUCCESS);
}

void runFromFileAction(const std::string &TestFile, unsigned Port) {
  // To exit
  signal(SIGINT, exitHandler);
  std::ifstream InFile(TestFile);
  auto TestData = nlohmann::json::parse(InFile);

  auto Test = TestData.template get<TestSystem::TestPaper>();
  runTest(Test, Port);
}

static void runFromFileAction() {
  std::cout << "Введите путь до файла с тестом:\n";
  std::string FilePath;
  getline(std::cin, FilePath);

  runFromFileAction(FilePath, getPort());
}

void chooseAction() {
  // To exit
  signal(SIGINT, exitHandler);
  std::cout << "Какое действие сделать? (Напишите число)\n"
               "  1 - Создать тест и запустить его\n"
               "  2 - Создать тест и сохранить в файле\n"
               "  3 - Запустить тест из файлa\n";
  auto Ans = getUnsignedFromStr();
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

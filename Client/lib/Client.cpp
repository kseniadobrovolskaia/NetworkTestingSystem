#include "Client.hpp"

namespace TestClient {

void failWithError(const std::string &Msg) {
  throw std::logic_error("\n\n" + Msg);
}

static void printAnswers(const std::vector<unsigned> &Answers) {
  std::for_each(Answers.begin(), Answers.end(),
                [](const auto &Ans) { std::cout << Ans + 1 << " "; });
  std::cout << "\n";
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

static sockaddr_in fillServerData(unsigned Port,
                                  const std::string &ServerHostname) {
  auto Server = gethostbyname(ServerHostname.c_str());
  if (Server == NULL)
    failWithError("Error, no such host\n");
  sockaddr_in ServerAddr;
  bzero((char *)&ServerAddr, sizeof(ServerAddr));
  ServerAddr.sin_family = AF_INET;
  bcopy((char *)Server->h_addr, (char *)&ServerAddr.sin_addr.s_addr,
        Server->h_length);
  ServerAddr.sin_port = htons(Port);
  return ServerAddr;
}

static std::vector<unsigned>
getAnswersFromGUIWindow(const TestSystem::TestPaper &Test) {
  int Argc = 0;
  char **Argv;
  Glib::RefPtr<Gtk::Application> Application =
      Gtk::Application::create(Argc, Argv, "org.gtkmm.example");

  GUI::TestWindow Window(Test);
  std::cout << "Please choose answers on test in window :D\n\n";
  Application->run(Window);

  return Window.getFinalResults();
}

void startTesting(unsigned Port, const std::string &ServerHostname) {
  auto SockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (SockFd < 0)
    failWithError("Error opening socket");

  auto ServerAddr = fillServerData(Port, ServerHostname);

  // Connect to Server
  if (connect(SockFd, (sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
    failWithError("Error connecting");

  // Test recives
  constexpr auto MaxTestSize = 10000u;
  char TestStr[MaxTestSize];
  readWithCheck(SockFd, reinterpret_cast<char *>(TestStr), MaxTestSize,
                "Error reading from socket");
  nlohmann::json TestData = nlohmann::json::parse(TestStr);
  auto Test = TestData.template get<TestSystem::TestPaper>();

  // Answer using GUI
  auto FinalResults = getAnswersFromGUIWindow(Test);

  std::cout << "Your answers: ";
  printAnswers(FinalResults);

  // Give answers to the Server for checking
  writeWithCheck(SockFd, FinalResults.data(), sizeof(FinalResults),
                 "Error writing to socket");

  // Recive rignt answers
  std::vector<unsigned> RightAnswers(FinalResults.size());
  readWithCheck(SockFd, RightAnswers.data(), sizeof(FinalResults),
                "Error reading from socket");

  std::cout << "Right answers: ";
  printAnswers(RightAnswers);

  // Recive mark
  char Mark;
  readWithCheck(SockFd, &Mark, /* size */ 1, "Error reading from socket");

  std::cout << "\nYour mark " << Mark << "!\n";

  close(SockFd);
}

} // namespace TestClient

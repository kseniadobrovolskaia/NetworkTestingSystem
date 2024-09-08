#include "Client.hpp"

namespace TestClient {

void failWithError(const std::string &Msg) {
  throw std::logic_error("\n\n" + Msg);
}

void startTesting(unsigned Port, const std::string &ServerHostname) {
  int sockfd, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    failWithError("ERROR opening socket");
  server = gethostbyname(ServerHostname.c_str());
  if (server == NULL)
    failWithError("ERROR, no such host\n");
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(Port);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    failWithError("ERROR connecting");

  // Test recives

  char str[10000];
  size_t size;
  n = read(sockfd, reinterpret_cast<char *>(str), sizeof(str));
  nlohmann::json data = nlohmann::json::parse(str);
  if (n < 0)
    failWithError("ERROR reading from socket");

  std::cout << "Please choose answers on test in window :D\n\n";
  // Convert json format test to GUI and open GUI_Window

  int argc = 0;
  char **argv;
  Glib::RefPtr<Gtk::Application> app =
      Gtk::Application::create(argc, argv, "org.gtkmm.example");

  auto Test = data.template get<TestSystem::TestPaper>();
  GUI::TestWindow Window(Test);

  // Answers using GUI
  app->run(Window);

  // Send results
  auto FinalResults = Window.getFinalResults();
  n = write(sockfd, FinalResults.data(), sizeof(FinalResults));
  if (n < 0)
    failWithError("ERROR writing to socket");

  std::cout << "Your answers: ";
  for (auto A : FinalResults)
    std::cout << A + 1 << " ";

  // Recive rignt answers
  std::vector<unsigned> right_answers(FinalResults.size());
  n = read(sockfd, right_answers.data(), sizeof(FinalResults));
  if (n < 0)
    failWithError("ERROR reading from socket");

  std::cout << "\nRight answers: ";
  for (auto A : right_answers)
    std::cout << A + 1 << " ";
  std::cout << "\n";

  // Recive mark
  char mark[2];
  n = read(sockfd, mark, 1);
  mark[1] = '\0';
  if (n < 0)
    failWithError("ERROR reading from socket");

  std::cout << "\n\nYour mark " << mark << "!\n";

  close(sockfd);
}

} // namespace TestClient

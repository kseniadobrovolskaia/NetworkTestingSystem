#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>


#include "TestSystem.h"

void failWithError(const char *Message) {
    perror(Message);
    exit(1);
}

static int calculateMark(std::string final_results, std::string right_answers) {
  int Mark = 5;
  for (int i = 0; i < 5; ++i) {
    if (final_results[i * 2] != right_answers[i * 2])
      --Mark;
  }
  return Mark;
}       

void createAction() {
  std::cout << "Введите вопрос номер 1\n";
}

void saveAction() {
  std::cout << "Введите вопрос номер 1\n";
}

void runFromFileAction() {
  std::cout << "Введите путь до файла с тестом\n";
  std::string FilePath;
  std::cin >> FilePath;

  std::ifstream f(FilePath);
  nlohmann::json data = nlohmann::json::parse(f);

  std::cout << "Json: \n" << data << std::endl;
  
  auto Test = data.template get<TestSystem::TestPaper>();
  std::cout << "Object: \n";
  Test.print();

}

static void chooseAction() {
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
  }
}


int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if (argc < 2)
    failWithError("ERROR, no port provided\n");

  chooseAction();


  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    failWithError("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
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

  // Send test
  n = write(newsockfd, "I send test in json format\n", 28);
  if (n < 0) 
    failWithError("ERROR writing to socket");
  
  // Read Answer
  char final_results[10];
  bzero(final_results,10);
  n = read(newsockfd,final_results,10);
  if (n < 0) 
    failWithError("ERROR reading from socket");
  std::cout << "Here is the answer: " << final_results << "\n";
  
  std::string right_answers = "3 2 2 3 1\0"; 
  // Send right answers
  n = write(newsockfd, right_answers.c_str(), 10);
  if (n < 0) 
    failWithError("ERROR writing to socket");
  
  // Send mark
  n = write(newsockfd, std::to_string(calculateMark(final_results, right_answers)).c_str(), 1);
  if (n < 0) 
    failWithError("ERROR writing to socket");

  close(newsockfd);
  close(sockfd);
  return 0; 
}

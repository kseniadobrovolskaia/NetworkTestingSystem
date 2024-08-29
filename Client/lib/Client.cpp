#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "GUI_Window.h"
#include <gtkmm/application.h>

void failWithError(const char *Message) {
    perror(Message);
    exit(1);
}


int main(int argc, char *argv[]) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " HOSTNAME PORT\n";
    exit(0);
  }

  int PORT = atoi(argv[2]);
  const char *HOST_NAME = argv[1];
  
  argc = 1;
  portno = PORT;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    failWithError("ERROR opening socket");
  server = gethostbyname(HOST_NAME);
  if (server == NULL)
    failWithError("ERROR, no such host\n");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    failWithError("ERROR connecting");
  
  
  // Test recives
  char buffer[256];
  bzero(buffer,256);
  n = read(sockfd,buffer,255);
  if (n < 0) 
    failWithError("ERROR reading from socket");
  std::cout << buffer << "\n";
  
  std::cout << "Please choose answers on test in window :D\n\n";
  // Convert json format test to GUI and open GUI_Window

  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  ExampleWindow window;

  // Answers using GUI
  app->run(window);
  
  // Send results
  const char *final_results = window.getResults().c_str();
  n = write(sockfd, final_results, 10);
  if (n < 0) 
    failWithError("ERROR writing to socket");


  std::cout << "Your answers: " << final_results << "\n";

  std::cout << "\nRight answers: ";

  // Recive rignt answers
  char right_answers[10];
  bzero(right_answers, 10);
  n = read(sockfd,right_answers,10);
  if (n < 0) 
    failWithError("ERROR reading from socket");
  std::cout << right_answers << "\n";
  
  // Recive mark
  char mark[2];
  n = read(sockfd, mark ,1);
  mark[1] = '\0';
  if (n < 0) 
    failWithError("ERROR reading from socket");
  
  std::cout << "\n\nYour mark " << mark << "!\n";
  
  close(sockfd);
  return 0;
}


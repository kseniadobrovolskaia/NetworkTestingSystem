#include <fstream>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "TestSystem.h"

namespace TestServer {

void failWithError(const std::string &Msg);

int calculateMark(std::vector<unsigned> ClientAnswers,
                  std::vector<unsigned> CorrectAnswers);

void chooseAction();
void createAction();
void saveAction();
void runFromFileAction();

} // namespace TestServer

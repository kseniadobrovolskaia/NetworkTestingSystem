#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "GUI_Window.h"
#include <gtkmm/application.h>

namespace TestClient {

void failWithError(const std::string &Msg);

void startTesting(unsigned Port, const std::string &ServerHostname);

} // namespace TestClient

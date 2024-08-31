#include "Server.hpp"

int main() {
  try {
    TestServer::chooseAction();
  } catch (std::exception &Ex) {
    std::cout << Ex.what() << "\n";
    exit(EXIT_FAILURE);
  }

  return 0;
}

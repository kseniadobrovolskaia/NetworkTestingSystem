#include "Client.hpp"

void failWithError(const char *Message) {
  perror(Message);
  exit(1);
}

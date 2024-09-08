#include "Client.hpp"
#include <getopt.h>

namespace TestClient {

static std::optional<const char *> ServerHostname;
static std::optional<unsigned> Port;

// clang-format off
static struct option CmdLineOpts[] = {
    {"help",             no_argument,        0,  'h'},
    {"port",             required_argument,  0,  'p'},
    {"server-hostname",  required_argument,  0,  's'},
    {0,                  0,                  0,   0 }};
// clang-format on

static void printHelp(const char *ProgName, int ErrorCode) {
  std::cerr << "USAGE:     " << ProgName << "   [options]\n\n";
  std::cerr << "OPTIONS: \n";
  struct option *opt = CmdLineOpts;
  while (opt->name) {
    if (isprint(opt->val))
      std::cerr << "\t   -" << static_cast<char>(opt->val) << "\t --"
                << opt->name << "\n";
    else
      std::cerr << "\t     \t --" << opt->name << "\n";
    opt++;
  }
  exit(ErrorCode);
}

/**
 * @brief setValue - it helps process incorrect values, which are given
 *                   for unsigned argument.
 */
static void setValue(const char *NameValue, const char *GivenValue,
                     std::optional<unsigned> &SetVar) {
  // find sign
  while (isspace((unsigned char)*GivenValue))
    GivenValue++;
  char Sign = *GivenValue;
  char *Endptr; //  Store the location where conversion stopped
  unsigned TryValue = strtoul(GivenValue, &Endptr, /* base */ 10);

  if (GivenValue == Endptr)
    failWithError("Invalid " + std::string(NameValue) + " " +
                  std::string(GivenValue) + " provided");
  else if (*Endptr)
    failWithError("Extra text after " + std::string(NameValue));
  else if (Sign == '-' && TryValue != 0)
    failWithError("Negative " + std::string(NameValue));
  SetVar = TryValue;
}

/**
 * @brief parseCmdLine - it parses the command line arguments.
 */
static void parseCmdLine(int Argc, char **Argv) {
  int NextOpt;
  while (true) {
    NextOpt = getopt_long(Argc, Argv,
                          "h"
                          "p:"
                          "s:",
                          CmdLineOpts, NULL);
    if (NextOpt == -1)
      break;
    switch (NextOpt) {
    case 'p':
      setValue("port", optarg, Port);
      break;
    case 'h':
      printHelp(Argv[0], 0);
      break;
    case 's':
      ServerHostname = optarg;
      break;
    case '?':
      printHelp(Argv[0], 1);
      break;
    }
  }
  if (!Port.has_value()) {
    std::cerr << "Port must be provided\n\n";
    printHelp(Argv[0], 1);
  }
  if (!ServerHostname.has_value()) {
    ServerHostname = "localhost";
    std::cerr << "Server-hostname must be provided. Using default hostname: "
                 "localhost\n\n";
  }
}

} // namespace TestClient

int main(int Argc, char *Argv[]) {
  try {
    TestClient::parseCmdLine(Argc, Argv);
    TestClient::startTesting(TestClient::Port.value(),
                             TestClient::ServerHostname.value());
  } catch (std::exception &Ex) {
    std::cout << Ex.what() << "\n";
    exit(EXIT_FAILURE);
  }

  return 0;
}

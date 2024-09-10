#include "Server.hpp"
#include <getopt.h>

namespace TestServer {

static std::optional<std::string> TestFile;
static std::optional<unsigned> Port;

// clang-format off
static option CmdLineOpts[] = {
    {"help",             no_argument,        0,  'h'},
    {"port",             required_argument,  0,  'p'},
    {"file-with-test",   required_argument,  0,  'f'},
    {0,                  0,                  0,   0 }};
// clang-format on

static void printHelp(const std::string &ProgName, int ErrorCode) {
  std::cerr << "USAGE:     " << ProgName << "   [options]\n\n";
  std::cerr << "OPTIONS: \n";
  auto Opt = CmdLineOpts;
  while (Opt->name) {
    if (isprint(Opt->val))
      std::cerr << "\t   -" << static_cast<char>(Opt->val) << "\t --"
                << Opt->name << "\n";
    else
      std::cerr << "\t     \t --" << Opt->name << "\n";
    ++Opt;
  }
  exit(ErrorCode);
}

/**
 * @brief getUnsignedFromStr - it helps get number and process incorrect values,
 *                             which are given for unsigned argument.
 */
static unsigned getUnsignedFromStr(const std::string &NameValue,
                                   const std::string &GivenValue) {
  // find sign
  auto FirstLetter = GivenValue.begin();
  while (isspace((unsigned char)*FirstLetter))
    ++FirstLetter;
  char Sign = *FirstLetter;
  char *Endptr; //  Store the location where conversion stopped
  auto TryValue = strtoul(GivenValue.c_str() +
                              std::distance(GivenValue.begin(), FirstLetter),
                          &Endptr, /* base */ 10);

  if (GivenValue == Endptr)
    failWithError("Invalid " + NameValue + " provided");
  else if (*Endptr)
    failWithError("Extra text after " + NameValue);
  else if (Sign == '-' && TryValue != 0)
    failWithError("Negative " + NameValue);
  return TryValue;
}

/**
 * @brief parseCmdLine - it parses the command line arguments.
 */
static void parseCmdLine(int Argc, char **Argv) {
  while (true) {
    auto NextOpt = getopt_long(Argc, Argv,
                               "h"
                               "p:"
                               "f:",
                               CmdLineOpts, NULL);
    if (NextOpt == -1)
      break;
    switch (NextOpt) {
    case 'p':
      Port = getUnsignedFromStr("port", optarg);
      break;
    case 'h':
      printHelp(Argv[0], 0);
      break;
    case 'f':
      TestFile = optarg;
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
  if (!TestFile.has_value()) {
    std::cerr << "File with test must be provided\n\n";
    printHelp(Argv[0], 1);
  }
}

} // namespace TestServer

int main(int Argc, char *Argv[]) {
  try {
    if (Argc > 1) {
      TestServer::parseCmdLine(Argc, Argv);
      TestServer::runFromFileAction(TestServer::TestFile.value(),
                                    TestServer::Port.value());
    } else
      TestServer::chooseAction();
  } catch (std::exception &Ex) {
    std::cout << Ex.what() << "\n";
    exit(EXIT_FAILURE);
  }

  return 0;
}

#include "TestSystem.h"
#include <fstream>

int main(int argc, char *argv[]) {
  std::ifstream f("example.json");
  nlohmann::json data = nlohmann::json::parse(f);

  std::cout << "Json: \n" << data << std::endl;
  
  auto Test = data.template get<TestSystem::TestPaper>();
  std::cout << "Object: \n";
  Test.print();

  return 0; 
}


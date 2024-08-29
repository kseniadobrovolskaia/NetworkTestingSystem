#include "TestSystem.h"

namespace TestSystem {

void to_json(nlohmann::json &J, const Question &Q) {
  Q.toJson(J);
}

void from_json(const nlohmann::json &J, Question &Q) {
  Q.fromJson(J);
}

 
void to_json(nlohmann::json &J, const TestPaper &T) {
  T.toJson(J);
}

void from_json(const nlohmann::json &J, TestPaper &T) {
  T.fromJson(J);
}


} // namespace TestSystem

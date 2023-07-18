#include "async.h"

int main(int argc, const char* argv[]) {
  int n{0};

  BlockState start;
  start.subscribe(std::make_shared<Log>());
  start.subscribe(std::make_shared<File>());
  if (argc > 0 && argc < 3) {
    n = std::stoi(argv[1]);
    start.set_state(n);
  } else {
    std::cout << "Error: wrong data" << std::endl;
  }
  return 0;
}

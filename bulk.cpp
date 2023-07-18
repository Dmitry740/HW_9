#include "async.h"

void print_file(const std::vector<std::string>& queue) {
  if (queue.empty()) {
    return;
  }
  long long res_time{0};

  std::string filename;

  if (queue.size() > 0) {
    auto start = std::chrono::high_resolution_clock::now();

    auto w = queue[0];

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    res_time = elapsed_ms.count();
    filename = "bulk" + std::to_string(res_time) + "_" +
               std::to_string(std::rand()) + ".log";

    std::ofstream out_file{filename, std::ios_base::app};
    if (!out_file) {
      std::cout << "Failed to open file: " << filename << std::endl;
    }

    for (auto q = 0; q != queue.size(); ++q) {
      out_file << queue[q];
      if (q != queue.size() - 1) {
        out_file << ", ";
      }
    }
    std::cout << std::endl;
  }
}

void print_log(const std::vector<std::string>& queue) {
  if (queue.empty()) {
    return;
  }

  std::cout << "bulk: ";

  for (size_t q = 0; q != queue.size(); ++q) {
    std::cout << queue[q];
    if (q != queue.size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << std::endl;
}

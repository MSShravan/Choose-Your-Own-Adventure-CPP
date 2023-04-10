#include "cyoa.hpp"

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <story pages directory>" << std::endl;
    exit(EXIT_FAILURE);
  }

  return startStory(argv[1]);
}

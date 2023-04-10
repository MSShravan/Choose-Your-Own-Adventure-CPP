#include "cyoa.hpp"

int main(int argc, char ** argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " <story directory>" << std::endl;
    exit(EXIT_FAILURE);
  }

  try {
    Book book;
    if (parseStory(argv[1], book) == EXIT_SUCCESS) {
      std::cout << book;
      return EXIT_SUCCESS;
    }
  }
  catch (std::exception & e) {
    std::cerr << "Error parsing the story!" << std::endl;
  }
  return EXIT_FAILURE;
}

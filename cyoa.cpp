#include "cyoa.hpp"

#include <climits>

bool isValidPageType(const char c, TYPE & pageType) {
  switch (c) {
    case 'N':
      pageType = N;
      break;
    case 'W':
      pageType = W;
      break;
    case 'L':
      pageType = L;
      break;
    default:
      return false;
  }
  return true;
}

//checks if a string is blank
bool isBlank(const std::string str) {
  for (size_t i = 0; i < str.size(); i++) {
    if (!isblank(str[i]))
      return false;
  }
  return true;
}

//Parses pagenum and variable for a choice
int parsePageNumAndVariable(const std::string str,
                            size_t & pageNum,
                            std::pair<std::string, long int> & variable) {
  const std::string delims("[=]");
  size_t beg = 0, pos = 0, count = 0;
  char * endptr;
  while ((beg = str.find_first_not_of(delims, pos)) != std::string::npos && count < 3) {
    pos = str.find_first_of(delims, beg + 1);

    if (count == 0 && str[pos] == delims.at(0)) {
      pageNum = strtoul(str.substr(beg, pos - beg).c_str(), &endptr, 10);
      if (*endptr != '\0' || (pageNum == ULONG_MAX && errno == ERANGE)) {
        std::cerr << "Page number must be of type size_t and not more than " << ULONG_MAX
                  << "!" << std::endl;
        return EXIT_FAILURE;
      }
    }
    else if (count == 1) {
      if (str[pos] == delims.at(1))
        variable.first = str.substr(beg, pos - beg);
      else if (str[pos] == delims.at(2)) {
        variable.first = "";
        count = 2;
      }
    }
    if (count == 2 && str[pos] == delims.at(2)) {
      variable.second = strtol(str.substr(beg, pos - beg).c_str(), &endptr, 10);
      if (*endptr != '\0' || (variable.second == LONG_MAX && errno == ERANGE)) {
        std::cerr << "Variable value must be of type long int and not more than "
                  << LONG_MAX << "!" << std::endl;
        return EXIT_FAILURE;
      }
    }
    count++;
  }

  try {
    if (count != 3 || (count == 3 && !isBlank(str.substr(beg)))) {
      std::cerr << std::endl
                << "Format error!! Use either of the string formats: "
                   "pagenum:destpage:text or pagenum[var=value]:destpage:text"
                << std::endl;
      return EXIT_FAILURE;
    }
  }
  catch (std::out_of_range & e) {
    return EXIT_SUCCESS;
  }

  return EXIT_SUCCESS;
}

//Adds an entry(page, choice, variable) into the book object
int updateBook(Book & book, std::vector<std::string> metadata, const bool isVar) {
  char * endptr;
  size_t number = strtoul(metadata[0].c_str(), &endptr, 10);
  std::pair<std::string, long int> variable("shravan", 0);
  if (*endptr != '\0') {
    if (parsePageNumAndVariable(metadata[0], number, variable) != EXIT_SUCCESS)
      return EXIT_FAILURE;
  }
  else if (number == ULONG_MAX && errno == ERANGE) {
    std::cerr << "Page number must be of type size_t and not more than " << ULONG_MAX
              << "!" << std::endl;
    return EXIT_FAILURE;
  }

  TYPE pageType = F;
  size_t destPage = strtoul(metadata[1].c_str(), &endptr, 10);
  if (*endptr != '\0' || metadata[1].compare("") == 0) {
    if (isVar) {
      long int value = strtol(metadata[2].c_str(), &endptr, 10);
      if (*endptr != '\0' || (value == LONG_MAX && errno == ERANGE)) {
        std::cerr << "Variable value must be of type long int and not more than "
                  << LONG_MAX << "!" << std::endl;
        return EXIT_FAILURE;
      }

      book.addVariable(number, metadata[1], value);
      return EXIT_SUCCESS;
    }
    else if (metadata[1].size() == 1 && !isValidPageType(metadata[1][0], pageType)) {
      std::cerr << "Page type must be either of N, W, L!" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else if (destPage == ULONG_MAX && errno == ERANGE) {
    std::cerr << "Destination page number must be of type size_t and not more than "
              << ULONG_MAX << "!" << std::endl;
    return EXIT_FAILURE;
  }

  if (pageType != F) {
    Page page(number, pageType, book.getStoryDir() + "/" + metadata[2]);
    try {
      book.updatePages(page);
    }
    catch (std::exception & e) {
      return EXIT_FAILURE;
    }
  }
  else {
    Choice choice(number, destPage, metadata[2], variable);
    book.updateChoices(number, choice);
  }

  return EXIT_SUCCESS;
}

//Parses a line into page and choice
void parseStoryLine(const std::string line,
                    std::vector<std::string> & metadata,
                    size_t & count,
                    const std::string delims) {
  size_t beg = 0, pos = 0;
  while ((beg = line.find_first_not_of(delims, pos)) != std::string::npos && count < 3) {
    pos = line.find_first_of(delims, beg + 1);
    if (count == 1) {
      if (beg - 2 >= 0 && line.substr(beg - 2, 2).compare(delims) == 0) {
        metadata.push_back("");
        metadata.push_back(line.substr(beg));
        count = 3;
        return;
      }
      else if (line[pos] != delims.at(1))
        return;
    }
    if (count != 2)
      metadata.push_back(line.substr(beg, pos - beg));
    else
      metadata.push_back(line.substr(beg));
    count++;
  }
  if (count == 2 && line[pos] == ':') {
    metadata.push_back("");
    count++;
  }
}

//Reads the story.txt file for parsing
int parseStory(const char * dir, Book & book, const bool remember) {
  std::string storyDir = std::string(dir);
  std::string storyFilePath = storyDir + "/story.txt";

  book.setStoryDir(storyDir);

  std::ifstream storyFile(storyFilePath.c_str());
  if (!storyFile.is_open()) {
    std::cerr << "is_open(): Error while opening the \"" << storyFilePath << "\" file!"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string line;

  while (std::getline(storyFile, line)) {
    if (!line.empty() && line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
      size_t count = 0;
      std::vector<std::string> metadata;
      bool isVar = false;
      parseStoryLine(line, metadata, count, "@::");
      if (count != 3) {
        if (remember && count == 1) {
          count = 0;
          metadata.clear();
          parseStoryLine(line, metadata, count, "$=");
          if (count != 3) {
            storyFile.close();
            std::cerr
                << std::endl
                << "Format error!! Use any of string formats: number@type:filename or "
                   "pagenum:destpage:text or pagenum$var=value "
                << std::endl;
            return EXIT_FAILURE;
          }
          isVar = true;
        }
        else {
          storyFile.close();
          std::cerr << std::endl
                    << "Format error!! Use either of the string formats: "
                       "number@type:filename or pagenum:destpage:text"
                    << std::endl;
          return EXIT_FAILURE;
        }
      }
      if (updateBook(book, metadata, isVar) != EXIT_SUCCESS)
        return EXIT_FAILURE;
    }
  }

  storyFile.close();
  return EXIT_SUCCESS;
}

//Starts the story and accepts user input to continue
int startStory(const char * dir, const bool remember) {
  Book book;
  try {
    if (parseStory(dir, book, remember) == EXIT_SUCCESS) {
      book.validateReferences();
    }
    else
      return EXIT_FAILURE;
  }
  catch (std::exception & e) {
    std::cerr << "Error parsing the story!" << std::endl;
    return EXIT_FAILURE;
  }

  size_t destPage = 0;
  std::cout << book[destPage];
  book.updateMemory(destPage);
  std::set<size_t> unavailable;
  if (book[destPage].getType() == W || book[destPage].getType() == L)
    return EXIT_SUCCESS;

  std::string line;
  bool read = true;
  size_t choice = 0;
  while (read) {
    std::getline(std::cin, line);
    if (line.empty() || line.find_first_not_of(" \t\n\v\f\r") == std::string::npos)
      continue;
    char * endptr;
    choice = strtoul(line.c_str(), &endptr, 10);
    if (*endptr != '\0' || (choice == ULONG_MAX && errno == ERANGE) || choice == 0) {
      std::cout << "That is not a valid choice, please try again" << std::endl;
    }
    else {
      if (unavailable.count(choice - 1) > 0) {
        std::cout << "That choice is not available at this time, please try again"
                  << std::endl;
        continue;
      }
      try {
        destPage = book[destPage].getDestPageForChoice(choice - 1);
        if (!remember)
          std::cout << book[destPage];
        else {
          book.updateMemory(destPage);
          unavailable = book[destPage].printPage(book.getMemory());
        }
        if (book[destPage].getType() == W || book[destPage].getType() == L)
          read = false;
      }
      catch (std::exception & e) {
        std::cout << "That is not a valid choice, please try again" << std::endl;
      }
    }
  }

  return EXIT_SUCCESS;
}

#include <cstdlib>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

#include "choice.hpp"

//represents the page type
typedef enum { N, W, L, F } TYPE;

class Page {
 private:
  size_t number;
  TYPE type;
  std::string filename;

  std::vector<Choice> choices;

 public:
  //Default constructor
  Page() : number(0), type(F), filename(""), choices() {}

  //Parameter constructor
  Page(size_t number_, TYPE type_, std::string filename_, std::vector<Choice> choices_) :
      number(number_), type(type_), filename(filename_), choices(choices_) {}

  Page(size_t number_, TYPE type_, std::string filename_) :
      number(number_), type(type_), filename(filename_), choices() {}

  //Copy constructor
  Page(const Page & rhs) :
      number(rhs.number), type(rhs.type), filename(rhs.filename), choices(rhs.choices) {}

  //Assignment operator
  Page & operator=(const Page & rhs) {
    if (this != &rhs) {
      number = rhs.number;
      type = rhs.type;
      filename.clear();
      filename = rhs.filename;
      choices.clear();
      for (size_t i = 0; i < rhs.choices.size(); i++) {
        choices.push_back(rhs.choices[i]);
      }
    }
    return *this;
  }

  //Destructor
  ~Page() {
    filename.clear();
    choices.clear();
  }

  size_t getNumber() const { return number; }

  TYPE getType() const { return type; }

  //Check if file exists
  void checkFileExists() const {
    std::ifstream page(filename.c_str());
    if (!page.is_open()) {
      std::cerr << "is_open(): Error while opening the \"" << filename << "\" file!"
                << std::endl;
      throw std::exception();
    }
    page.close();
  }

  size_t getDestPageForChoice(const size_t index) const {
    if (index < 0 || index >= choices.size()) {
      //std::cerr << "Choice index out of range!" << std::endl;
      throw std::exception();
    }
    return choices[index].getDestPage();
  }

  void updateChoices(const Choice choice) { choices.push_back(choice); }

  std::vector<size_t> getDestPages() const {
    std::vector<size_t> destPages;

    for (size_t i = 0; i < choices.size(); i++)
      destPages.push_back(choices[i].getDestPage());

    return destPages;
  }

  size_t getIndexOfDestPage(const size_t destPage) {
    size_t index = 0;
    for (size_t i = 0; i < choices.size(); i++) {
      if (choices[i].getDestPage() == destPage) {
        index = i + 1;
        break;
      }
    }
    return index;
  }

 private:
  //Print page
  void printPage(std::ostream & s) const {
    std::ifstream page(filename.c_str());
    if (!page.is_open()) {
      std::cerr << "is_open(): Error while opening the \"" << filename << "\" file!"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    s << page.rdbuf();
    page.close();
  }

  //Print choices
  void printChoices(std::ostream & s) const {
    s << "What would you like to do?" << std::endl << std::endl;
    if (choices.size() > 0) {
      for (size_t i = 0; i < choices.size(); i++) {
        std::stringstream choiceStr;
        choiceStr << " " << i + 1 << ". " << choices[i];
        s << choiceStr.str() << std::endl;
        choiceStr.flush();
      }
    }
  }

 public:
  //<< operator
  friend std::ostream & operator<<(std::ostream & s, const Page & page) {
    page.printPage(s);
    s << std::endl;

    switch (page.type) {
      case N:
        page.printChoices(s);
        break;
      case W:
        s << "Congratulations! You have won. Hooray!" << std::endl;
        break;
      case L:
        s << "Sorry, you have lost. Better luck next time!" << std::endl;
        break;
      default:
        break;
    }

    return s;
  }

  //Prints story page and presents with available choices for user interaction
  std::set<size_t> printPage(const std::map<std::string, long int> memory) const {
    std::ifstream page(filename.c_str());
    if (!page.is_open()) {
      std::cerr << "is_open(): Error while opening the \"" << filename << "\" file!"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    std::cout << page.rdbuf() << std::endl;
    page.close();

    std::set<size_t> unavailable;
    if (type == N) {
      std::cout << "What would you like to do?" << std::endl << std::endl;
      for (size_t i = 0; i < choices.size(); i++) {
        std::stringstream choiceStr;
        choiceStr << " " << i + 1 << ". ";
        if ((memory.count(choices[i].getVariable().first) > 0 &&
             memory.at(choices[i].getVariable().first) !=
                 choices[i].getVariable().second) ||
            (memory.count(choices[i].getVariable().first) == 0 &&
             choices[i].getVariable().first != "shravan" &&
             choices[i].getVariable().second != 0)) {
          choiceStr << "<UNAVAILABLE>";
          unavailable.insert(i);
        }
        else {
          choiceStr << choices[i];
        }
        std::cout << choiceStr.str() << std::endl;
        choiceStr.flush();
      }
    }
    else if (type == W) {
      std::cout << "Congratulations! You have won. Hooray!" << std::endl;
    }
    else if (type == L) {
      std::cout << "Sorry, you have lost. Better luck next time!" << std::endl;
    }
    return unavailable;
  }

  void toString() {
    std::cout << "number: " << number << std::endl;
    switch (type) {
      case N:
        std::cout << "type: N" << std::endl;
        break;
      case W:
        std::cout << "type: W" << std::endl;
        break;
      case L:
        std::cout << "type: L" << std::endl;
        break;
      default:
        std::cout << "type: F" << std::endl;
    }
    std::cout << "filename: " << filename << std::endl;

    std::cout << "choices: " << std::endl;
    typename std::vector<Choice>::const_iterator it = choices.begin();
    while (it != choices.end()) {
      std::cout << *it << std::endl;
      ++it;
    }
  }
};

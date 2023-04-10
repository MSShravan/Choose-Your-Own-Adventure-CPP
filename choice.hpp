#include <iostream>
#include <string>

class Choice {
 private:
  size_t pagenum;
  size_t destpage;
  std::string text;
  std::pair<std::string, long int> variable;

 public:
  //Default constructor
  //Default initializing pair "variable" with first parameter as "shravan"(randomly chosen default value)
  Choice() : pagenum(0), destpage(0), text(""), variable("shravan", 0) {}

  //Parameter constructor
  Choice(size_t pagenum_,
         size_t destpage_,
         std::string text_,
         std::pair<std::string, long int> variable_) :
      pagenum(pagenum_), destpage(destpage_), text(text_), variable(variable_) {}

  //Copy constructor
  Choice(const Choice & rhs) :
      pagenum(rhs.pagenum),
      destpage(rhs.destpage),
      text(rhs.text),
      variable(rhs.variable) {}

  //Assignment operator
  Choice & operator=(const Choice & rhs) {
    if (this != &rhs) {
      pagenum = rhs.pagenum;
      destpage = rhs.pagenum;
      text.clear();
      text = rhs.text;
      variable = rhs.variable;
    }
    return *this;
  }

  //Destructor
  ~Choice() { text.clear(); }

  size_t getDestPage() const { return destpage; }

  std::pair<std::string, long int> getVariable() const { return variable; }

  void toString() {
    std::cout << "pagenum: " << pagenum << std::endl
              << "destpage: " << destpage << std::endl
              << "text: " << text << std::endl
              << "variable: (" << variable.first << ", " << variable.second << ")"
              << std::endl;
  }

  friend std::ostream & operator<<(std::ostream & s, const Choice & choice) {
    s << choice.text;
    return s;
  }
};

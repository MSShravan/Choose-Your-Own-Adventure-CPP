#include <set>
#include <stack>

#include "page.hpp"

class Book {
 private:
  std::string storyDir;
  std::vector<Page> pages;
  std::set<size_t> pageNums;
  std::set<size_t> destPageNums;
  std::set<size_t> winPageNums;
  std::set<size_t> losePageNums;
  std::vector<std::vector<size_t> > graph;
  std::map<size_t, std::map<std::string, long int> > variables;
  std::map<std::string, long int> memory;

 public:
  //Default constructor
  Book() :
      storyDir(""),
      pages(),
      pageNums(),
      destPageNums(),
      winPageNums(),
      losePageNums(),
      graph(),
      variables(),
      memory() {}

  //Parameter constructor
  Book(std::vector<std::vector<size_t> > graph_,
       std::map<size_t, std::map<std::string, long int> > variables_,
       std::map<std::string, long int> memory_) :
      graph(), variables(), memory(memory_) {
    for (size_t i = 0; i < graph_.size(); i++)
      graph.push_back(graph_[i]);

    std::map<size_t, std::map<std::string, long int> >::const_iterator jt =
        variables_.begin();
    while (jt != variables_.end()) {
      std::map<std::string, long int> temp = jt->second;
      variables.insert(std::make_pair(jt->first, temp));
      ++jt;
    }
  }

  //Copy constructor
  Book(const Book & rhs) :
      storyDir(rhs.storyDir),
      pages(rhs.pages),
      pageNums(rhs.pageNums),
      destPageNums(rhs.destPageNums),
      winPageNums(rhs.winPageNums),
      losePageNums(rhs.losePageNums),
      graph(),
      variables(),
      memory(rhs.memory) {
    for (size_t i = 0; i < rhs.graph.size(); i++)
      graph.push_back(rhs.graph[i]);

    std::map<size_t, std::map<std::string, long int> >::const_iterator jt =
        rhs.variables.begin();
    while (jt != rhs.variables.end()) {
      std::map<std::string, long int> temp = jt->second;
      variables.insert(std::make_pair(jt->first, temp));
      ++jt;
    }
  }

  //Assignment operator
  Book & operator=(const Book & rhs) {
    if (this != &rhs) {
      pages.clear();
      for (size_t i = 0; i < rhs.pages.size(); i++) {
        pages.push_back(rhs.pages[i]);
      }
      storyDir.clear();
      storyDir = rhs.storyDir;
      pageNums.clear();
      pageNums = rhs.pageNums;
      destPageNums.clear();
      destPageNums = rhs.destPageNums;
      winPageNums.clear();
      winPageNums = rhs.winPageNums;
      losePageNums.clear();
      losePageNums = rhs.losePageNums;

      std::vector<std::vector<size_t> > temp;
      for (size_t i = 0; i < rhs.graph.size(); i++)
        temp.push_back(rhs.graph[i]);

      for (size_t i = 0; i < graph.size(); i++)
        graph[i].clear();
      graph.clear();
      graph = temp;

      std::map<size_t, std::map<std::string, long int> >::iterator it = variables.begin();
      while (it != variables.end()) {
        it->second.clear();
        ++it;
      }
      variables.clear();
      std::map<size_t, std::map<std::string, long int> >::const_iterator jt =
          rhs.variables.begin();
      while (jt != rhs.variables.end()) {
        std::map<std::string, long int> temp = jt->second;
        variables.insert(std::make_pair(jt->first, temp));
        ++jt;
      }

      memory.clear();
      memory = rhs.memory;
    }
    return *this;
  }

  //Destructor
  ~Book() {
    pages.clear();
    storyDir.clear();
    pageNums.clear();
    destPageNums.clear();
    winPageNums.clear();
    losePageNums.clear();

    for (size_t i = 0; i < graph.size(); i++)
      graph[i].clear();
    graph.clear();

    std::map<size_t, std::map<std::string, long int> >::iterator it = variables.begin();
    while (it != variables.end()) {
      it->second.clear();
      ++it;
    }
    variables.clear();

    memory.clear();
  }

  const Page & operator[](size_t index) const {
    if (index < 0 || index >= pages.size()) {
      //std::cerr << "Page number out of range!" << std::endl;
      throw std::exception();
    }
    return pages[index];
  }

  void setStoryDir(const std::string storyDir_) {
    storyDir.clear();
    storyDir = storyDir_;
  }

  std::string getStoryDir() { return storyDir; }

  void updatePages(const Page page) {
    if (page.getNumber() != pages.size()) {
      std::cerr << "Page declarations must appear in order. Expected " << pages.size()
                << " but found " << page.getNumber() << "!" << std::endl;
      throw std::exception();
    }
    page.checkFileExists();
    pages.push_back(page);
    pageNums.insert(page.getNumber());
    if (page.getType() == W)
      winPageNums.insert(page.getNumber());
    else if (page.getType() == L)
      losePageNums.insert(page.getNumber());
    graph.push_back(page.getDestPages());
  }

  void updateChoices(const size_t pageNum, const Choice choice) {
    for (size_t i = 0; i < pages.size(); i++) {
      if (pages[i].getNumber() == pageNum) {
        if (winPageNums.count(pageNum) > 0 || losePageNums.count(pageNum) > 0) {
          std::cerr << "Choice declared for W/L page: pagenum \"" << pageNum << "\""
                    << std::endl;
          throw std::exception();
        }

        pages[i].updateChoices(choice);
        destPageNums.insert(choice.getDestPage());
        graph[pageNum] = pages[i].getDestPages();
        return;
      }
    }

    std::cerr << "Invalid choice definition: pagenum \"" << pageNum << "\" not found!"
              << std::endl;
    throw std::exception();
  }

  void addVariable(const size_t pageNum,
                   const std::string varName,
                   const long int value) {
    if (pageNums.count(pageNum) > 0) {
      if (variables.size() == 0 || variables.count(pageNum) == 0) {
        std::map<std::string, long int> varMap;
        varMap.insert(std::make_pair(varName, value));
        variables.insert(std::make_pair(pageNum, varMap));
      }
      else {
        if (variables[pageNum].insert(std::make_pair(varName, value)).second == false) {
          variables[pageNum].erase(varName);
          variables[pageNum].insert(std::make_pair(varName, value));
        }
      }
    }
    else {
      std::cerr << "Invalid choice definition: pagenum \"" << pageNum
                << "\" not declared!" << std::endl;
      throw std::exception();
    }
  }

  std::map<std::string, long int> getMemory() { return memory; }

  void updateMemory(const size_t pageNum) {
    if (variables.count(pageNum) > 0)
      memory.insert(variables[pageNum].begin(), variables[pageNum].end());
  }

  //Validating page references for story as a whole
  void validateReferences() {
    if (pageNums.count(0) == 0) {
      std::cerr << "Page 0 not found!" << std::endl;
      throw std::exception();
    }

    std::set<size_t> destPageNumsCopy(destPageNums);
    std::set<size_t>::iterator it = pageNums.begin();
    while (it != pageNums.end()) {
      if (*it != 0 && destPageNumsCopy.count(*it) == 0) {
        std::cerr << "Reference not found for page number: \"" << *it << "\""
                  << std::endl;
        throw std::exception();
      }
      destPageNumsCopy.erase(*it);
      ++it;
    }
    if (destPageNumsCopy.size() != 0) {
      std::cerr << "Contains invalid references!" << std::endl;
      throw std::exception();
    }
    if (winPageNums.size() == 0 || losePageNums.size() == 0) {
      std::cerr << "There must be atleast one WIN and a LOSE page!" << std::endl;
      throw std::exception();
    }
  }

  bool inCurrPath(const std::vector<size_t> & currPath, const size_t & currNode) {
    for (size_t i = 0; i < currPath.size() - 1; i++)
      if (currPath[i] == currNode)
        return true;

    return false;
  }

  //Overloaded method to print paths to win
  void printPathsToWin(const std::set<std::vector<size_t> > & winPaths) {
    for (std::set<std::vector<size_t> >::iterator it = winPaths.begin();
         it != winPaths.end();
         ++it) {
      std::vector<size_t> currPath = *it;
      for (size_t i = 0; i < currPath.size(); i++) {
        std::cout << currPath[i];
        if (i != currPath.size() - 1)
          std::cout << "(" << pages[currPath[i]].getIndexOfDestPage(currPath[i + 1])
                    << "),";
        else
          std::cout << "(win)";
      }
      std::cout << std::endl;
    }
  }

  //Uses DFS graph searching algorithm to find all the winning paths
  void printPathsToWin() {
    size_t from = 0;
    bool noWin = true;
    std::set<std::vector<size_t> > winPaths;
    for (std::set<size_t>::iterator it = winPageNums.begin(); it != winPageNums.end();
         ++it) {
      size_t to = *it;

      std::stack<std::vector<size_t> > todo;
      std::set<size_t> visited;
      std::vector<size_t> fromPath(1, from);
      todo.push(fromPath);

      while (!todo.empty()) {
        std::vector<size_t> currPath = todo.top();
        todo.pop();
        size_t currNode = currPath.back();

        if (currNode == to) {
          winPaths.insert(currPath);
          noWin = false;
        }
        else if (!inCurrPath(currPath, currNode) || visited.count(currNode) == 0) {
          visited.insert(currNode);
          for (std::vector<size_t>::iterator jt = graph[currNode].begin();
               jt != graph[currNode].end();
               ++jt) {
            std::vector<size_t> newPath(currPath);
            newPath.push_back(*jt);
            todo.push(newPath);
          }
        }
      }
    }

    if (noWin)
      std::cout << "This story is unwinnable!" << std::endl;
    else
      printPathsToWin(winPaths);
  }

  //<< operator
  friend std::ostream & operator<<(std::ostream & s, const Book & book) {
    for (size_t i = 0; i < book.pages.size(); i++) {
      std::stringstream pageStr;
      pageStr << "Page " << book.pages[i].getNumber() << std::endl
              << "==========" << std::endl;
      s << pageStr.str();
      pageStr.flush();

      s << book.pages[i];
    }
    return s;
  }

  void toString() {
    std::cout << "storyDir: " << storyDir << std::endl;
    std::cout << "pages: " << std::endl;
    for (size_t i = 0; i < pages.size(); i++) {
      std::cout << pages[i];
    }
  }
};

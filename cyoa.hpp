#ifndef __CYOA_H__
#define __CYOA_H__

#include "book.hpp"

//variable "remember" is used so that story could remember things that happened on other pages (for step 4)
int parseStory(const char * dir, Book & book, const bool remember = false);
int startStory(const char * dir, const bool remember = false);
#endif

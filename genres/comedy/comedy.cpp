#include "./comedy.hpp"

#include <iostream>
#include <sstream>

std::string Comedy::process(const std::string& val) {
  std::stringstream ss;
  int counter = 1;
  char c;
  for (std::size_t i = 0; i < val.length(); ++i) {
    c = val[i];
    if (c != ' ' && c != '\n') {
      if (counter % 2 == 0) {
        c = std::toupper(c);
      }
      ++counter;
    } else {
      counter = 1;
    }
    ss << c;
  }
  
  return ss.str();
}

std::string Comedy::get_genre_name() {
  return ct::comedy_tag;
}
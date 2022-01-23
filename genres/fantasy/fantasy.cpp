#include "./fantasy.hpp"

#include <iostream>
#include <sstream>

std::string Fantasy::process(const std::string& val) {
  std::stringstream ss;
  char c;
  bool new_word = true;

  for (std::size_t i = 0; i < val.length(); ++i) {
    c = val[i];
    if (c == ' ' || c == '\n') {
      new_word = true;
    } else {
      if (std::isalpha(c) && std::isupper(c) == false) {
        if (new_word == true) {
          c = std::toupper(c);
        }
      }
      new_word = false;
    }
    ss << c;
  }

  return ss.str();
}

std::string Fantasy::get_genre_name() {
  return ct::fantasy_tag;
}

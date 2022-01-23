#include "./horror.hpp"

#include <iostream>
#include <sstream>

std::string Horror::process(const std::string& val) {
  std::stringstream ss;
  char c;

  for (std::size_t i = 0; i < val.length(); ++i) {
    c = val[i];
    if (std::isalpha(c) && this->isVowel(c) == false) {
      ss << c << (char)std::tolower(c);
    } else {
      ss << c;
    }
  }

  return ss.str();
}

bool Horror::isVowel(const char c) {
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u'
    || c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

std::string Horror::get_genre_name() {
  return ct::horror_tag;
}

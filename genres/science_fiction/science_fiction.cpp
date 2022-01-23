#include "./science_fiction.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

std::string ScienceFiction::process(const std::string& val) {
  std::stringstream ss;
  std::stringstream word;
  std::string word_s;
  char c;
  int word_count = 1;

  for (std::size_t i = 0; i < val.length(); ++i) {
    c = val[i];
    if (c == ' ' || c == '\n') {
      word_s = word.str();
      word.str("");
      if (word_count % 7 == 0) {
        std::reverse(word_s.begin(), word_s.end());
      }
      ss << word_s << c;
      if (c == ' ') {
        ++word_count;
      } else {
        word_count = 1;
      }
    } else {
      word << c;
    }
  }
  // check if there is one more
  if (word.str().length() != 0) {
    word_s = word.str();
    word.str("");
    if (word_count % 7 == 0) {
      std::reverse(word_s.begin(), word_s.end());
    }
    ss << word_s;
  }

  return ss.str();
}

std::string ScienceFiction::get_genre_name() {
  return ct::science_fiction_tag;
}

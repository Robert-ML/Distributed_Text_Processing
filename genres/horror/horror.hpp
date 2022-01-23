#ifndef GENRES_HORROR_HORROR_HPP
#define GENRES_HORROR_HORROR_HPP

#include "../genre.hpp"

#include <string>

class Horror : public Genre {
 public:
  Horror(const int rank) : Genre(rank) { }

 private:
  std::string get_genre_name() override;
  static bool isVowel(const char c);

  std::string process(const std::string& val) override;
};

#endif // GENRES_HORROR_HORROR_HPP
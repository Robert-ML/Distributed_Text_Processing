#ifndef GENRES_FANTASY_FANTASY_HPP
#define GENRES_FANTASY_FANTASY_HPP

#include "../genre.hpp"

#include <string>

class Fantasy : public Genre {
 public:
  Fantasy(const int rank) : Genre(rank) { }

 private:
  std::string get_genre_name() override;

  std::string process(const std::string& val) override;
};

#endif // GENRES_FANTASY_FANTASY_HPP